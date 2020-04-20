/*
 * Shift Pin: Pin 0
 * Store Pin: Pin 1
 * Data Pin: Pin 2
*/
#define shiftPin 0 // this is the ClockPin
#define storePin 1 
#define datenPin 2 //datePin seems to be allready in use for shiftOut
#define resetPin 3 // resetPin is till now not in use

#define LEDSize 8 //The Maximum of LEDs is 8


unsigned short Farben[LEDSize] = {0};
unsigned long lastAnimStep = 0; //rewrite with a boolean array.


void setup() {
  
  // set all pins to output
  
  pinMode(shiftPin, OUTPUT);
  pinMode(storePin, OUTPUT);
  pinMode(datenPin, OUTPUT);
  pinMode(resetPin, OUTPUT);

  digitalWrite(resetPin, LOW); //TODO reset if low or High?
}

void loop() {

  // generate a random led view every 200ms
  if (millis() - lastAnimStep > 200) {
    // look wirt a timer for the 200ms
    lastAnimStep = millis();

    // generate all 4 areas
    for (int i = 0; i < LEDSize; i++)
    {
      Farben[i] = random(pow(2,LEDSize*LEDSize)); //generates all Posible states of one Stage (LED on or off)
      //Farben[i] = 65535;

    }
  }


  // Display on the LED Cube
  for (int e = 0; e < LEDSize; e++)
  {
    int e_activation = 0;
    switch (e)
    {
      case 0: e_activation = 0b00000001; break; //long is 32 bit... not 8
      case 1: e_activation = 0b00000010; break;
      case 2: e_activation = 0b00000100; break;
      case 3: e_activation = 0b00001000; break;
      case 4: e_activation = 0b00010000; break;
      case 5: e_activation = 0b00100000; break;
      case 6: e_activation = 0b01000000; break;
      case 7: e_activation = 0b10000000; break;
    }
    
    // shift the data out
    digitalWrite(storePin, LOW);
    shiftOut(datenPin, shiftPin, MSBFIRST, e_activation);
    shiftOut(datenPin, shiftPin, MSBFIRST, Farben[e] >> 8); //Todo muessen hier jetzt 16 geschoben werden? Bisher haetten doch eigentlich nur 4 geschoben werden muessen
    shiftOut(datenPin, shiftPin, MSBFIRST, Farben[e]);
    digitalWrite(1, HIGH);

//TODO ist hier wirklich der ResetPin gemeint?
    digitalWrite(resetPin, LOW);// Output Enable activate
    delay(2);//display the area for one millisecond
    digitalWrite(resetPin, HIGH);// Output Enable deactivate
  }
}

