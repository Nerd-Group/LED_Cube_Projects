#include "Time.h"

#define Z1 6
#define Z2 9
#define Z3 10
#define Z4 11

#define X1Y1 1
#define X1Y2 0
#define X1Y3 2
#define X1Y4 3

#define X2Y1 8
#define X2Y2 7
#define X2Y3 5
#define X2Y4 4

#define X3Y1 12
#define X3Y2 13
#define X3Y3 14
#define X3Y4 15

#define X4Y1 17
#define X4Y2 19
#define X4Y3 18
#define X4Y4 16

struct{
  bool aktiv = false;
  int Koordinaten[3] = {0};
}schlange[64];

int Bewegungsrichtung[3] = {0};
int Futter[3] = {0};
int LetztesElement[3] = {0};
int SekundeAlt, SekundeNeu;
int Helligkeit = 255;
int Helligkeitstufen[32] = {0, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 8, 10, 11, 13, 16, 19, 23, 27, 32, 38, 45, 54, 64, 76, 91, 108, 128, 152, 181, 215, 255};

void setup() {

  //Zufallsgenerator setzen
  randomSeed(analogRead(A7));
  
  //Schlange initialisieren und auf Startposition setzen
  schlange[0].aktiv = true;
  schlange[0].Koordinaten[0] = 1;
  schlange[0].Koordinaten[1] = 1;
  schlange[0].Koordinaten[2] = 1;
  
  //Alle Pins, die mit LEDs verbundne sind, auf Ausgange stellen
  for(int i = 0; i < 20; i++)
  {
    pinMode(i, OUTPUT);
  }
    
  //Alle Pins ausschalten
  AllePinsAusschalten();

  //Futter auf Zufallsposition setzen
  FutterAufZufallsposition();

  //Zufällige Bewegungsrichtung einstellen
  ZufallsBewegungsrichtung();

  //Uhrzeit für die Time-Library setzen
  setTime( 0, 0, 0, 0, 0, 0 );

  //SekundeNeu initialisieren
  SekundeNeu = second();
}

void loop() {
  
  //Den vorherigen Wert in SekundeALt speichern
  SekundeAlt = SekundeNeu;

  //Sekunde Neu aktualisieren
  SekundeNeu = second();

  //Falls sich die Zeit nicht geändert hat
  if(SekundeAlt == SekundeNeu)
  {
    //Helligkeit der LEDs ermitteln
    HelligkeitErmitteln();

    //Schlange ausgeben
    Ausgeben();

    //Bewegungsrichtung ermitteln, falls ein Schritt ausgeführt werden muss
    BewegungsrichtungErmitteln();
  }
  //Falls eine Sekunde vorbei ist
  else
  {
    
    //Die Schlange um einen Schritt bewegen
    SchrittAusfuehren();

    //Falls das Futter gegessen wurde
    if(PruefeFutterGegessen())
    {
      //Verlängere die Schlange
      SchlangeWaechst();

      //Setze das Futter auf eine neue Zufallsposition
      FutterAufZufallsposition();
    }

    //falls die Schlange sich selbst erwischt hat, ist das Spiel aus
    if(PruefeObSpielVorbei())
    {
      SpielVorbei();
    }
  }
}

void AllePinsAusschalten(void)
{
  for(int i = 0; i < 20; i++)
  {
    if((i != 6) && (i != 9) && (i != 10) && (i != 11))
    digitalWrite(i,LOW);
  }
  //Die 4 Ebenen ausschalten:
  analogWrite(6, 255);
  analogWrite(9, 255);
  analogWrite(10, 255);
  analogWrite(11, 255);
}

void FutterAufZufallsposition( void )
{
  boolean ZufallspositionGefunden;
  do
  {
    ZufallspositionGefunden = true;
    
    //Zufällige Koordinaten vergeben:
    for(int i = 0; i < 3; i++)
    {
      Futter[i] = random(4) + 1;
    }

    //Pruefen, ob diese Koordiaten die Schlange nicht berühren
    for(int i = 0; i < 64 && schlange[i].aktiv && ZufallspositionGefunden; i++)
    {
      if(Futter[0] == schlange[i].Koordinaten[0] && Futter[1] == schlange[i].Koordinaten[1] && Futter[2] == schlange[i].Koordinaten[2])
      {
        //Falls das doch der Fall sein sollte, muss eine neue Zufallsposition her
        ZufallspositionGefunden = false;
      }
    }
  }while(!ZufallspositionGefunden);
}

void ZufallsBewegungsrichtung(void)
{
  //Bewegungsrichtung auf 0 setzen
  for(int i = 0; i < 3; i++)
  {
    Bewegungsrichtung[i] = 0;
  }

  //Eine zufällige Koordinate auswählen: X-, Y-oder Z-Richtung
  int Zufallskoordinate = random(3);

  //Bestimmen, ob die Schlange sich ins Negative oder ins Positive bewegt
  int Zufallsrichtung = random(2);
  if(Zufallsrichtung == 0)
  {
    Zufallsrichtung = 1;
  }
  else
  {
    Zufallsrichtung = -1;
  }

  //Bewegungsrichtung in der globalen Varibale speichern
  Bewegungsrichtung[Zufallskoordinate] = Zufallsrichtung;
}

void HelligkeitErmitteln(void)
{
  //Potentiometerstellung auslesen
  int ADC_Wert = analogRead(A7);
  
  //In 32 einzelne Schritte unterteilen
  Helligkeit = map(ADC_Wert, 0, 1023, 0, 31);

  //Dem Schritt sienen Helligkeitswert zuweisen
  Helligkeit = Helligkeitstufen[ Helligkeit ];

  // Den Helligkeitswert invertieren
  Helligkeit = 255 - Helligkeit;
}

void Ausgeben(void)
{
  //Alle LEDs ausschalten
  AllePinsAusschalten();

  //Jede Ebene einmal durchgehen
  for(int i = 1; i < 5; i++)
  {
    //Für jede einzelne LED der schlange prüfen, ob sie auf der Ebene liegt
    for(int j = 0; j < 64 && schlange[j].aktiv; j++)
    {
      //Falls das der falls ist ...
      if(schlange[j].Koordinaten[2] == i)
      {
        // .. Wird die richtige LED auf der X- und Y-Achse eingeschaltet
        LEDsSetzen(schlange[j].Koordinaten[0], schlange[j].Koordinaten[1]);
      }
    }

    //Falls das Futter auch auf dieser Ebene lisgt, soll es auch angezeigt werden
    if(Futter[2] == i)
    {
      LEDsSetzen(Futter[0],Futter[1]);
    }

    //Passende Ebene einschalten
    switch(i)
    {
      case 1: analogWrite(6, Helligkeit); break;
      case 2: analogWrite(9, Helligkeit); break;
      case 3: analogWrite(10, Helligkeit); break;
      case 4: analogWrite(11, Helligkeit); break;
    }
    
    //Warten, bis zur nächsten Reihe gewechselt wird
    delay(1);

    //Ebene ausschalten
    switch(i)
    {
      case 1: analogWrite(6, 255); break;
      case 2: analogWrite(9, 255); break;
      case 3: analogWrite(10, 255); break;
      case 4: analogWrite(11, 255); break;
    }
    
    //Alle LEds wieder ausschalten
    AllePinsAusschalten();
  }
}

void LEDsSetzen(int Xposition, int Yposition)
{
  //Prüfe mit einer Verschalchtelung von zwei switches, welcher Pin gemeint ist
  int Pin = 0;
  switch(Xposition)
  {
    case 1:
      switch(Yposition)
      {
        case 1: Pin = X1Y1; break;
        case 2: Pin = X1Y2; break;
        case 3: Pin = X1Y3; break;
        case 4: Pin = X1Y4; break;
      }
      break;
    case 2:
      switch(Yposition)
      {
        case 1: Pin = X2Y1; break;
        case 2: Pin = X2Y2; break;
        case 3: Pin = X2Y3; break;
        case 4: Pin = X2Y4; break;
      }
      break;
    case 3:
      switch(Yposition)
      {
        case 1: Pin = X3Y1; break;
        case 2: Pin = X3Y2; break;
        case 3: Pin = X3Y3; break;
        case 4: Pin = X3Y4; break;
      }
      break;
    case 4:
      switch(Yposition)
      {
        case 1: Pin = X4Y1; break;
        case 2: Pin = X4Y2; break;
        case 3: Pin = X4Y3; break;
        case 4: Pin = X4Y4; break;
      }
      break;
  }
  //Nachdem der Pin erfasst wurde, wird er eingeschaltet
  digitalWrite(Pin, HIGH);
}

void BewegungsrichtungErmitteln(void)
{
  //Abfragen, welche Taste auf der Steuerung gedrückt ist
  int Taste = TasteGedrueckt();

  //Neue Bewegungsrichtung aus der gedrükten taste ermitteln
  int BewegungsrichtungNeu[3] = {0};

  //die neue Bewegungsrichtung darf nicht entgegengesetzt der alten Bewegungsrichtung verlaufen
  bool BewegungsrichtungAendern = true;
  
  switch(Taste)
  {
    case 1: BewegungsrichtungNeu[0] = 1; break;//Taste 1: nach links / X-Koordinate positiv
    case 2: BewegungsrichtungNeu[0] = -1; break;//Taste 1: nach rechts / X-Koordinate negativ
    case 3: BewegungsrichtungNeu[1] = 1; break;//Taste 2: nach vorne / Y-Koordinate positiv
    case 4: BewegungsrichtungNeu[1] = -1; break;//Taste 2: nach hinten / Y-Koordinate negativ
    case 5: BewegungsrichtungNeu[2] = 1; break;//Taste 3: nach oben / Z-Koordinate positiv
    case 6: BewegungsrichtungNeu[2] = -1; break;//Taste 3: nach unten / Z-Koordinate negativ
    default: BewegungsrichtungAendern = false; break; //Nihts ändern
  }

  //Prüfung für die X-Achse:
  if((BewegungsrichtungNeu[0] == 1 && Bewegungsrichtung[0] == -1) || (BewegungsrichtungNeu[0] == -1 && Bewegungsrichtung[0] == 1))
  {
    BewegungsrichtungAendern = false;
  }
  //Prüfung für die Y-Achse:
  if((BewegungsrichtungNeu[1] == 1 && Bewegungsrichtung[1] == -1) || (BewegungsrichtungNeu[1] == -1 && Bewegungsrichtung[1] == 1))
  {
    BewegungsrichtungAendern = false;
  }
  //Prüfung für die Z-Achse:
  if((BewegungsrichtungNeu[2] == 1 && Bewegungsrichtung[2] == -1) || (BewegungsrichtungNeu[2] == -1 && Bewegungsrichtung[2] == 1))
  {
    BewegungsrichtungAendern = false;
  }

  //Falls die Bewegungsrichtung zulässig ist, soll sie geändert werden
  if(BewegungsrichtungAendern)
  {
    for(int i =  0; i < 3; i++)
    {
      Bewegungsrichtung[i] = BewegungsrichtungNeu[i];
    }
  }
}

int TasteGedrueckt(void)
{
  int Taste = 0;

  //AD-Wandler bei Steuerung einlesen
  int ADC_Wert = analogRead(A6);

  //Aus dem Wert des AD-Wandlers die Taste bestimmen
  if(ADC_Wert > 850 && ADC_Wert < 855)
  {
    Taste = 1;
  }
  if(ADC_Wert > 725 && ADC_Wert < 735)
  {
    Taste = 4;
  }
  if(ADC_Wert > 460 && ADC_Wert < 465)
  {
    Taste = 3;
  }
  if(ADC_Wert > 635 && ADC_Wert < 640)
  {
    Taste = 2;
  }
  if(ADC_Wert > 560 && ADC_Wert < 570)
  {
    Taste = 6;
  }
  if(ADC_Wert > 505 && ADC_Wert < 515)
  {
    Taste = 5;
  }

  //Die gedrückte Taste zurückgeben
  return Taste;
}

void SchrittAusfuehren(void)
{
  //Für jede Achse einmal wiederholen
  for(int i = 0; i < 3; i++)
  {
    //Koordinaten des Schlangenkopfes sichern
    LetztesElement[i] = schlange[0].Koordinaten[i];

    //In die Richtige Richtung bewegen
    if(Bewegungsrichtung[i] == 1)
    {
      schlange[0].Koordinaten[i]++;
    }
    if(Bewegungsrichtung[i] == -1)
    {
      schlange[0].Koordinaten[i]--;
    }
    
    //Falls unterhalb des Würfels, ober hinsetzen
    if(schlange[0].Koordinaten[i] < 1)
    {
      schlange[0].Koordinaten[i] = 4;
    }

    //Falls oberhalb des Würfels, unten hinsetzen
    if(schlange[0].Koordinaten[i] > 4)
    {
      schlange[0].Koordinaten[i] = 1;
    }
  }

  //den Rest der Schlange weiterbewegen
  for(int i = 1; i < 64 && schlange[i].aktiv; i++)
  {
    //Für jede Achse einzeln ausführen
    for(int j = 0; j < 3; j++)
    {
      //Auf die Position des letzten Elementes bewegen
      int hilf = schlange[i].Koordinaten[j];
      schlange[i].Koordinaten[j] = LetztesElement[j];
      LetztesElement[j] = hilf;
      
      //Falls unterhalb des Würfels, ober hinsetzen
      if(schlange[i].Koordinaten[j] < 1)
      {
        schlange[i].Koordinaten[j] = 4;
      }
  
      //Falls oberhalb des Würfels, unten hinsetzen
      if(schlange[i].Koordinaten[j] > 4)
      {
        schlange[i].Koordinaten[j] = 1;
      }
    }
  }
}

bool PruefeFutterGegessen(void)
{
  //Zuerst wird davon ausgeganen, dass die Schlange das Futter nicht gegessen hat
  bool gegessen = false;

  //Dann wird geprüft, ob der Schlangenkopf und das Futter dieselben Koordinaten haben
  if(schlange[0].Koordinaten[0] == Futter[0] && schlange[0].Koordinaten[1] == Futter[1] && schlange[0].Koordinaten[2] == Futter[2])
  {
    gegessen = true;
  }
  return gegessen;
}

void SchlangeWaechst(void)
{
  //zähle durch bis zu dem erten inaktiven Element im Array
  int i;
  for(i = 1; i < 64 && schlange[i].aktiv; i++){}

  //aktiviere dieses Element und setze es auf die richtige Position
  schlange[i].aktiv = true;
  schlange[i].Koordinaten[0] = LetztesElement[0];
  schlange[i].Koordinaten[1] = LetztesElement[1];
  schlange[i].Koordinaten[2] = LetztesElement[2];
}

bool PruefeObSpielVorbei(void)
{
  bool Verloren = false;

  //Gehe jedes Element der schlange hinter dem Schlangankopt durch
  for(int i = 1; i < 64 && schlange[i].aktiv && (!Verloren); i++)
  {
    //Überprüfe, ob der Schlangenkopf auf einem dieser Elemente aufliegt
    if(schlange[0].Koordinaten[0] == schlange[i].Koordinaten[0] && schlange[0].Koordinaten[1] == schlange[i].Koordinaten[1] && schlange[0].Koordinaten[2] == schlange[i].Koordinaten[2])
    {
      Verloren = true;
    }
  }
  return Verloren;
}

void SpielVorbei(void)
{
  //Zeige eine Animation in Endlosschleife
  while(true)
  {
    //Schhalte den Würfel aus
    AllePinsAusschalten();

    //Warte eine Sekunde
    delay(1000);

    //Schalte alle LEDs an
    for(int i = 0; i < 20; i++)
    {
      digitalWrite(i, HIGH);
    }
    //Warte eine Sekunde und aktualisiere währenddessen die Helliekeit
    for(int i = 0; i < 1000; i++)
    {
      HelligkeitErmitteln();
      analogWrite(6, Helligkeit);
      analogWrite(9, Helligkeit);
      analogWrite(10, Helligkeit);
      analogWrite(11, Helligkeit);
      delay(1);
    }
  }
}
