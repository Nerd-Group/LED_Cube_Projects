#include <Arduino.h>
#include <Wire.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "index.h"

#define ssid "HUAWEI P10 lite"
#define password "e1529ca42b93"

unsigned int localPort = 2390;

int shiftPin = D2;   // Pin SCK an 74HC595
int storePin = D1;   // PIN RCK an 74HC595
int dataPin  = D0;  // Pin SER an 74HC595

int brightness = 255;
int Helligkeitstufen[32] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 19, 23, 27, 32, 38, 45, 54, 64, 76, 91, 108, 128, 152, 181, 215, 255};
int counter = 0;
int SuperCounter = 0;
unsigned short Farben[4][3] = {0};
unsigned long lastAnimStep = 0;

int Mode = 0;


// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;



#define  false

WiFiClient client;
unsigned int last_poll = 0;

ESP8266WebServer server(80);

void handleRoot() {

  String response = MAIN_page;
  server.send(200, "text/html", response);

}

void power() {
  // Get Query Variables
  Mode = server.arg("mode").toInt();
  brightness = Helligkeitstufen[server.arg("brightness").toInt()];

  Serial.print("mode:");
  Serial.println(Mode);
  Serial.print("Brightness:");
  Serial.println(brightness);

  // ACK
  server.send ( 201, "text/plain", "");

  counter = 0;

}

void getStatus() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["status"] = "OK";
  root["uptime"] = millis();

  String json_string;
  root.printTo(json_string);
  server.send(200, "application/json", json_string);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(0) + server.argName(1) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {

  if (true)
    Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (true)
      Serial.print(".");
  }

  if (true) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  server.on("/", handleRoot);
  server.on("/power", power);

  server.onNotFound(handleNotFound);

  server.begin();
  if (true)
    Serial.println("Template HTTP server started");
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
  
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);

  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);


  pinMode(storePin, OUTPUT);  // Pin auf Output setzen
  pinMode(shiftPin, OUTPUT);  // Pin auf Output setzen
  pinMode(dataPin, OUTPUT);   // Pin auf Output setzen

}

void loop() {
  server.handleClient();
  unsigned long now = millis();
  if (last_poll + (1000 * 3) < now) {
    if (true) {
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    }
    last_poll = now;
  }

  if (millis() - lastAnimStep > 200) {
    lastAnimStep = millis();
    switch (Mode)
    {
      case 0: RandomAnimation(); break;
      case 1: Rainbow(); break;
      case 2: RGB_Animation(); break;
      case 3: Uni(); break;
      case 4: Rotation(); break;
      case 5: CubicCube(); break;
      case 6: MakerFaire(); break;
    }
  }
  Display_LED_Cube();
}


void Display_LED_Cube( void )
{
  for (int e = 0; e < 4; e++)
  {
    Ausgabe(Farben[e]);

    switch (e)
    {
      case 0: digitalWrite(D3, LOW); break;
      case 1: digitalWrite(D4, LOW); break;
      case 2: digitalWrite(D5, LOW); break;
      case 3: digitalWrite(D6, LOW); break;
    }
    delayMicroseconds(brightness * 8); //on time
    digitalWrite(D3, HIGH);
    digitalWrite(D4, HIGH);
    digitalWrite(D5, HIGH);
    digitalWrite(D6, HIGH);
    delayMicroseconds((255 - brightness) * 8); //off time
  }
}

void Ausgabe(unsigned short *Colors)
{
  digitalWrite(storePin, LOW);
  shiftOut(dataPin, shiftPin, MSBFIRST, Colors[0] >> 8);
  shiftOut(dataPin, shiftPin, MSBFIRST, Colors[0]);
  shiftOut(dataPin, shiftPin, MSBFIRST, Colors[1] >> 8);
  shiftOut(dataPin, shiftPin, MSBFIRST, Colors[1]);
  shiftOut(dataPin, shiftPin, MSBFIRST, Colors[2] >> 8);
  shiftOut(dataPin, shiftPin, MSBFIRST, Colors[2]);
  digitalWrite(storePin, HIGH);

  //Serial.println(String(Colors[0]) + " " + String(Colors[1]) + " " + String(Colors[2]));
}

void RandomAnimation()
{
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      Farben[i][j] = random(65535);
      //Serial.println(Farben[i][j]);
    }

  }
}


void RGB_Animation()
{
  switch (counter)
  {
    case 0:
      for (int i = 0; i < 4; i++)
      {
        Farben[i][0] = 65535;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
      }
      break;
    case 1:
      for (int i = 0; i < 4; i++)
      {
        Farben[i][0] = 0;
        Farben[i][1] = 65535;
        Farben[i][2] = 0;
      }
      break;
    case 2:
      for (int i = 0; i < 4; i++)
      {
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 65535;
      }
      break;
    case 3:
      for (int i = 0; i < 4; i++)
      {
        Farben[i][0] = 61440;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
      }
      break;
    case 4:
      for (int i = 0; i < 4; i++)
      {
        Farben[i][0] = 3840;
        Farben[i][1] = 61440;
        Farben[i][2] = 0;
      }
      break;
    case 5:
      for (int i = 0; i < 4; i++)
      {
        Farben[i][0] = 240;
        Farben[i][1] = 3840;
        Farben[i][2] = 61440;
      }
      break;
    case 6:
      for (int i = 0; i < 4; i++)
      {
        Farben[i][0] = 15;
        Farben[i][1] = 240;
        Farben[i][2] = 3840;
      }
      break;
    case 7:
      for (int i = 0; i < 4; i++)
      {
        Farben[i][0] = 0;
        Farben[i][1] = 15;
        Farben[i][2] = 240;
      }
      break;
    case 8:
      for (int i = 0; i < 4; i++)
      {
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 15;
      }
      break;
    case 9:
      for (int i = 0; i < 4; i++)
      {
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
      }
      break;
    case 10:
      for (int i = 1; i < 4; i++)
      {
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
      }
      Farben[0][0] = 61440;
      Farben[0][1] = 0;
      Farben[0][2] = 0;
      break;
    case 11:
      for (int i = 2; i < 4; i++)
      {
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
      }
      Farben[0][0] = 3840;
      Farben[0][1] = 61440;
      Farben[0][2] = 0;

      Farben[1][0] = 61440;
      Farben[1][1] = 0;
      Farben[1][2] = 0;
      break;
    case 12:
      Farben[0][0] = 240;
      Farben[0][1] = 3840;
      Farben[0][2] = 61440;

      Farben[1][0] = 3840;
      Farben[1][1] = 61440;
      Farben[1][2] = 0;

      Farben[2][0] = 61440;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 13:
      Farben[0][0] = 15;
      Farben[0][1] = 240;
      Farben[0][2] = 3840;

      Farben[1][0] = 240;
      Farben[1][1] = 3840;
      Farben[1][2] = 61440;

      Farben[2][0] = 3840;
      Farben[2][1] = 61440;
      Farben[2][2] = 0;

      Farben[3][0] = 61440;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 14:
      Farben[0][0] = 0;
      Farben[0][1] = 15;
      Farben[0][2] = 240;

      Farben[1][0] = 15;
      Farben[1][1] = 240;
      Farben[1][2] = 3840;

      Farben[2][0] = 240;
      Farben[2][1] = 3840;
      Farben[2][2] = 61440;

      Farben[3][0] = 3840;
      Farben[3][1] = 61440;
      Farben[3][2] = 0;
      break;
    case 15:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 15;

      Farben[1][0] = 0;
      Farben[1][1] = 15;
      Farben[1][2] = 240;

      Farben[2][0] = 15;
      Farben[2][1] = 240;
      Farben[2][2] = 3840;

      Farben[3][0] = 240;
      Farben[3][1] = 3840;
      Farben[3][2] = 61440;
      break;
    case 16:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 15;

      Farben[2][0] = 0;
      Farben[2][1] = 15;
      Farben[2][2] = 240;

      Farben[3][0] = 15;
      Farben[3][1] = 240;
      Farben[3][2] = 3840;
      break;
    case 17:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 15;

      Farben[3][0] = 0;
      Farben[3][1] = 15;
      Farben[3][2] = 240;
      break;
    case 18:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 15;
      break;
    case 19:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 20:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 15;
      break;
    case 21:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 15;

      Farben[3][0] = 0;
      Farben[3][1] = 15;
      Farben[3][2] = 240;
      break;
    case 22:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 15;

      Farben[2][0] = 0;
      Farben[2][1] = 15;
      Farben[2][2] = 240;

      Farben[3][0] = 15;
      Farben[3][1] = 240;
      Farben[3][2] = 3840;
      break;
    case 23:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 15;

      Farben[1][0] = 0;
      Farben[1][1] = 15;
      Farben[1][2] = 240;

      Farben[2][0] = 15;
      Farben[2][1] = 240;
      Farben[2][2] = 3840;

      Farben[3][0] = 240;
      Farben[3][1] = 3840;
      Farben[3][2] = 61440;
      break;
    case 24:
      Farben[0][0] = 0;
      Farben[0][1] = 15;
      Farben[0][2] = 240;

      Farben[1][0] = 15;
      Farben[1][1] = 240;
      Farben[1][2] = 3840;

      Farben[2][0] = 240;
      Farben[2][1] = 3840;
      Farben[2][2] = 61440;

      Farben[3][0] = 3840;
      Farben[3][1] = 61440;
      Farben[3][2] = 0;
      break;
    case 25:
      Farben[0][0] = 15;
      Farben[0][1] = 240;
      Farben[0][2] = 3840;

      Farben[1][0] = 240;
      Farben[1][1] = 3840;
      Farben[1][2] = 61440;

      Farben[2][0] = 3840;
      Farben[2][1] = 61440;
      Farben[2][2] = 0;

      Farben[3][0] = 61440;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 26:
      Farben[0][0] = 240;
      Farben[0][1] = 3840;
      Farben[0][2] = 61440;

      Farben[1][0] = 3840;
      Farben[1][1] = 61440;
      Farben[1][2] = 0;

      Farben[2][0] = 61440;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 27:
      for (int i = 2; i < 4; i++)
      {
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
      }
      Farben[0][0] = 3840;
      Farben[0][1] = 61440;
      Farben[0][2] = 0;

      Farben[1][0] = 61440;
      Farben[1][1] = 0;
      Farben[1][2] = 0;
      break;
    case 28:
      for (int i = 1; i < 4; i++)
      {
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
      }
      Farben[0][0] = 61440;
      Farben[0][1] = 0;
      Farben[0][2] = 0;
      break;
    case 29:
      for (int i = 0; i < 4; i++)
      {
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
      }
    case 30:
      for (int i = 0; i < 4; i++)
      {
        Farben[i][0] = 65535;
        Farben[i][1] = 65535;
        Farben[i][2] = 65535;
      }
      break;
  }
  counter++;

  if (counter > 30)
    counter = 0;
}

void Rainbow()
{
  switch (counter)
  {
    case 0:
      for (int i = 0; i < 4; i++)
      {
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
      }
      break;
    case 1:
      for (int i = 1; i < 4; i++)
      {
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
      }
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 65535;
      break;
    case 2:
      for (int i = 2; i < 4; i++)
      {
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
      }
      Farben[0][0] = 0;
      Farben[0][1] = 65535;
      Farben[0][2] = 65535;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 65535;
      break;
    case 3:
      Farben[0][0] = 0;
      Farben[0][1] = 65535;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 65535;
      Farben[1][2] = 65535;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 65535;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 4:
      Farben[0][0] = 65535;
      Farben[0][1] = 65535;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 65535;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 65535;
      Farben[2][2] = 65535;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 65535;
      break;
    case 5:
      Farben[0][0] = 65535;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 65535;
      Farben[1][1] = 65535;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 65535;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 65535;
      Farben[3][2] = 65535;
      break;
    case 6:
      Farben[0][0] = 65535;
      Farben[0][1] = 0;
      Farben[0][2] = 65535;

      Farben[1][0] = 65535;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 65535;
      Farben[2][1] = 65535;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 65535;
      Farben[3][2] = 0;
      break;
    case 7:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 65535;

      Farben[1][0] = 65535;
      Farben[1][1] = 0;
      Farben[1][2] = 65535;

      Farben[2][0] = 65535;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 65535;
      Farben[3][1] = 65535;
      Farben[3][2] = 0;
      break;
    case 8:
      Farben[0][0] = 0;
      Farben[0][1] = 65535;
      Farben[0][2] = 65535;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 65535;

      Farben[2][0] = 65535;
      Farben[2][1] = 0;
      Farben[2][2] = 65535;

      Farben[3][0] = 65535;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 9:
      Farben[0][0] = 0;
      Farben[0][1] = 65535;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 65535;
      Farben[1][2] = 65535;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 65535;

      Farben[3][0] = 65535;
      Farben[3][1] = 0;
      Farben[3][2] = 65535;
      break;
  }
  counter++;
  if (counter > 9)
    counter = 4;
}

void Uni()
{
  int color = random(100) % 3;
  for (int i = 0; i < 4; i++)
  {
    Farben[i][0] = 0;
    Farben[i][1] = 0;
    Farben[i][2] = 0;
    Farben[i][color] = 65535;
  }
}

void Rotation()
{
  for (int i = 0; i < 4; i++)
  {
    switch (counter)
    {
      case 0:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0b0000000000000001;
        break;
      case 1:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0b0000000000000011;
        break;
      case 2:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0b0000000000000111;
        break;
      case 3:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0b0000000000001111;
        break;
      case 4:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0b0000000010001111;
        break;
      case 5:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0b0000100010001111;
        break;
      case 6:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0b1000100010001111;
        break;
      case 7:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0b1100100010001111;
        break;
      case 8:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0b1110100010001111;
        break;
      case 9:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0b1111100010001111;
        break;
      case 10:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0b1111100110001111;
        break;
      case 11:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0B1111100110011111;
        break;
      case 12:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0B1111100110011110;
        break;
      case 13:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0B1111100110011100;
        break;
      case 14:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0B1111100110011000;
        break;
      case 15:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0B1111100110010000;
        break;
      case 16:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0B1111100100010000;
        break;
      case 17:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0B1111000100010000;
        break;
      case 18:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0B0111000100010000;
        break;
      case 19:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0B0011000100010000;
        break;
      case 20:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0B0001000100010000;
        break;
      case 21:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0B0000000100010000;
        break;
      case 22:
        Farben[i][0] = 0;
        Farben[i][1] = 0;
        Farben[i][2] = 0B0000000000010000;
        break;

      case 23:
        Farben[i][0] = 0;
        Farben[i][1] = 0b0000000000000001;
        Farben[i][2] = 0;
        break;
      case 24:
        Farben[i][0] = 0;
        Farben[i][1] = 0b0000000000000011;
        Farben[i][2] = 0;
        break;
      case 25:
        Farben[i][0] = 0;
        Farben[i][1] = 0b0000000000000111;
        Farben[i][2] = 0;
        break;
      case 26:
        Farben[i][0] = 0;
        Farben[i][1] = 0b0000000000001111;
        Farben[i][2] = 0;
        break;
      case 27:
        Farben[i][0] = 0;
        Farben[i][1] = 0b0000000010001111;
        Farben[i][2] = 0;
        break;
      case 28:
        Farben[i][0] = 0;
        Farben[i][1] = 0b0000100010001111;
        Farben[i][2] = 0;
        break;
      case 29:
        Farben[i][0] = 0;
        Farben[i][1] = 0b1000100010001111;
        Farben[i][2] = 0;
        break;
      case 30:
        Farben[i][0] = 0;
        Farben[i][1] = 0b1100100010001111;
        Farben[i][2] = 0;
        break;
      case 31:
        Farben[i][0] = 0;
        Farben[i][1] = 0b1110100010001111;
        Farben[i][2] = 0;
        break;
      case 32:
        Farben[i][0] = 0;
        Farben[i][1] = 0b1111100010001111;
        Farben[i][2] = 0;
        break;
      case 33:
        Farben[i][0] = 0;
        Farben[i][1] = 0b1111100110001111;
        Farben[i][2] = 0;
        break;
      case 34:
        Farben[i][0] = 0;
        Farben[i][1] = 0B1111100110011111;
        Farben[i][2] = 0;
        break;
      case 35:
        Farben[i][0] = 0;
        Farben[i][1] = 0B1111100110011110;
        Farben[i][2] = 0;
        break;
      case 36:
        Farben[i][0] = 0;
        Farben[i][1] = 0B1111100110011100;
        Farben[i][2] = 0;
        break;
      case 37:
        Farben[i][0] = 0;
        Farben[i][1] = 0B1111100110011000;
        Farben[i][2] = 0;
        break;
      case 38:
        Farben[i][0] = 0;
        Farben[i][1] = 0B1111100110010000;
        Farben[i][2] = 0;
        break;
      case 39:
        Farben[i][0] = 0;
        Farben[i][1] = 0B1111100100010000;
        Farben[i][2] = 0;
        break;
      case 40:
        Farben[i][0] = 0;
        Farben[i][1] = 0B1111000100010000;
        Farben[i][2] = 0;
        break;
      case 41:
        Farben[i][0] = 0;
        Farben[i][1] = 0B0111000100010000;
        Farben[i][2] = 0;
        break;
      case 42:
        Farben[i][0] = 0;
        Farben[i][1] = 0B0011000100010000;
        Farben[i][2] = 0;
        break;
      case 43:
        Farben[i][0] = 0;
        Farben[i][1] = 0B0001000100010000;
        Farben[i][2] = 0;
        break;
      case 44:
        Farben[i][0] = 0;
        Farben[i][1] = 0B0000000100010000;
        Farben[i][2] = 0;
        break;
      case 45:
        Farben[i][0] = 0;
        Farben[i][1] = 0B0000000000010000;
        Farben[i][2] = 0;
        break;

      case 46:
        Farben[i][0] = 0b0000000000000001;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 47:
        Farben[i][0] = 0b0000000000000011;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 48:
        Farben[i][0] = 0b0000000000000111;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 49:
        Farben[i][0] = 0b0000000000001111;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 50:
        Farben[i][0] = 0b0000000010001111;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 51:
        Farben[i][0] = 0b0000100010001111;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 52:
        Farben[i][0] = 0b1000100010001111;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 53:
        Farben[i][0] = 0b1100100010001111;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 54:
        Farben[i][0] = 0b1110100010001111;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 55:
        Farben[i][0] = 0b1111100010001111;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 56:
        Farben[i][0] = 0b1111100110001111;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 57:
        Farben[i][0] = 0B1111100110011111;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 58:
        Farben[i][0] = 0B1111100110011110;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 59:
        Farben[i][0] = 0B1111100110011100;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 60:
        Farben[i][0] = 0B1111100110011000;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 61:
        Farben[i][0] = 0B1111100110010000;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 62:
        Farben[i][0] = 0B1111100100010000;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 63:
        Farben[i][0] = 0B1111000100010000;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 64:
        Farben[i][0] = 0B0111000100010000;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 65:
        Farben[i][0] = 0B0011000100010000;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 66:
        Farben[i][0] = 0B0001000100010000;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 67:
        Farben[i][0] = 0B0000000100010000;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
      case 68:
        Farben[i][0] = 0B0000000000010000;
        Farben[i][1] = 0;
        Farben[i][2] = 0;
        break;
    }
  }
  counter++;
  if (counter > 68)
    counter = 0;
}

void CubicCube()
{
  switch (counter)
  {
    case 0:
      for (int i = 0; i < 4; i++)
      {
        for (int j = 0; j < 3; j++)
        {
          Farben[i][j] = 0;
        }
      }
      break;
    case 1:
      for (int i = 1; i < 4; i++)
      {
        for (int j = 0; j < 3; j++)
        {
          Farben[i][j] = 0;
        }
      }

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0B1000;
      break;
    case 2:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0B111011101110;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0B111010101110;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0B111011101110;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 3:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 65535;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0B1111100110011111;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0B1111100110011111;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 65535;
      break;
    case 4:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0B0111011101110000;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0B0111010101110000;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 0B0111011101110000;
      break;
    case 5:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0B11001100000000;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 0B11001100000000;
      break;
    case 6:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 0B1000000000000;
      break;
      
    case 7:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0B1000000000000000;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 8:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0B1100110000000000;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0B1100110000000000;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 9:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0B1110111011100000;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0B1110101011100000;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0B1110111011100000;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 10:
      Farben[0][0] = 65535;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0B1111100110011111;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0B1111100110011111;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 65535;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 11:
      Farben[0][0] = 0B11101110111;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0B11101010111;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0B11101110111;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 12:
      Farben[0][0] = 0B110011;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0B110011;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 13:
      Farben[0][0] = 0B1;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;

    case 14:
      Farben[0][0] = 0;
      Farben[0][1] = 0B1;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 15:
      Farben[0][0] = 0;
      Farben[0][1] = 0B110011;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0B110011;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 16:
      Farben[0][0] = 0;
      Farben[0][1] = 0B11101110111;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0B11101010111;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0B11101110111;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      break;
    case 17:
      Farben[0][0] = 0;
      Farben[0][1] = 65535;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0B1111100110011111;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0B1111100110011111;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 65535;
      Farben[3][2] = 0;
      break;
    case 18:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0B1110111011100000;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0B1110101011100000;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0B1110111011100000;
      Farben[3][2] = 0;
      break;
    case 19:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0B1100110000000000;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0B1100110000000000;
      Farben[3][2] = 0;
      break;
    case 20:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0B1000000000000000;
      Farben[3][2] = 0;
      break;
  }
  counter++;

  if (counter > 20)
    counter = 0;
}


void MakerFaire()
{
  switch(SuperCounter)
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      RandomAnimation();
      SuperCounter++;
      break;
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
      Rainbow();
      SuperCounter++;
      break;
    case 26:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 65535;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 65535;
      Farben[2][2] = 65535;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 65535;
      SuperCounter++;
      break;
    case 27:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 65535;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 65535;
      Farben[3][2] = 65535;
      SuperCounter++;
      break;
    case 28:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 65535;
      Farben[3][2] = 0;
      SuperCounter++;
      break;
    case 29:
      Farben[0][0] = 0;
      Farben[0][1] = 0;
      Farben[0][2] = 0;

      Farben[1][0] = 0;
      Farben[1][1] = 0;
      Farben[1][2] = 0;

      Farben[2][0] = 0;
      Farben[2][1] = 0;
      Farben[2][2] = 0;

      Farben[3][0] = 0;
      Farben[3][1] = 0;
      Farben[3][2] = 0;
      SuperCounter++;
      break;
    case 30:
      RGB_Animation();
      SuperCounter++;
      break;
    case 31:
      RGB_Animation();
      if(counter == 0)
      {
        SuperCounter++;
      }
      break;
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
      Uni();
      SuperCounter++;
      break;
    case 42:
      Rotation();
      SuperCounter++;
      break;
    case 43:
      Rotation();
      if(counter == 0)
      {
        SuperCounter++;
      }
      break;
    case 44:
      CubicCube();
      SuperCounter++;
      break;
    case 45:
      CubicCube();
      if(counter == 0)
      {
        SuperCounter = 0;
      }
      break;
  }
}

