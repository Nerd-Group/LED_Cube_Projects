/*
 * Shift Pin: Pin 0
 * Store Pin: Pin 1
 * Data Pin: Pin 2
*/

unsigned short Farben[4] = {0};
unsigned long lastAnimStep = 0;


void setup() {
  
  // set all pins to output
  
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  digitalWrite(3, LOW);
}

void loop() {

  // generate a random led view every 200ms
  if (millis() - lastAnimStep > 200) {
    // look wirt a timer for the 200ms
    lastAnimStep = millis();

    // generate all 4 areas
    for (int i = 0; i < 4; i++)
    {
      Farben[i] = random(65535);
      //Farben[i] = 65535;

    }
  }


  // Display on the LED Cube
  for (int e = 0; e < 4; e++)
  {
    int e_activation = 0;
    switch (e)
    {
      case 0: e_activation = 0b00000001; break;
      case 1: e_activation = 0b00000010; break;
      case 2: e_activation = 0b00000100; break;
      case 3: e_activation = 0b00001000; break;
    }
    
    // shift the data out
    digitalWrite(1, LOW);
    shiftOut(2, 0, MSBFIRST, e_activation);
    shiftOut(2, 0, MSBFIRST, Farben[e] >> 8);
    shiftOut(2, 0, MSBFIRST, Farben[e]);
    digitalWrite(1, HIGH);

    digitalWrite(3, LOW);// Output Enable activate
    delay(2);//display the area for one millisecond
    digitalWrite(3, HIGH);// Output Enable deactivate
  }
}

