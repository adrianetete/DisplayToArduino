/*
  Pines del Display
  1 -> DataPin
  2 -> ClockPin
  3 -> LatchPin
  4 -> GND
  5 -> VCC
*/

int dataPin = 2; // Amarillo
int latchPin = 3; // Verde
int clockPin = 4;// Rojo

int dataPinLED = 8; // Azul
int latchPinLED = 9; // Verde
int clockPinLED = 10; //Amarillo

byte trama[12];

// My shitfOut 8x3 bits
void shiftOutLEDS(int myDataPin, int myClockPin, byte myDataOut1, byte myDataOut2, byte myDataOut3) {

  //Setup inicial
  int i = 0;
  int pinState;

  //Enable data input
  digitalWrite(latchPinLED, 0);

  //Shift first byte for gear
  for (i = 0; i <= 7; i++)  {
    digitalWrite(myClockPin, 0);

    if ( myDataOut1 & (1 << i) ) {
      pinState = 1;
    }
    else {
      pinState = 0;
    }
    digitalWrite(myDataPin, pinState);
    digitalWrite(myClockPin, 1);
  }

  //Shift second byte for higher LEDs
  for (i = 0; i <= 7; i++)  {
    digitalWrite(myClockPin, 0);

    if ( myDataOut2 & (1 << i) ) {
      pinState = 1;
    }
    else {
      pinState = 0;
    }
    digitalWrite(myDataPin, pinState);
    digitalWrite(myClockPin, 1);
  }

  //Shift third byte for lower LEDs
  for (i = 0; i <= 7; i++)  {
    digitalWrite(myClockPin, 0);

    if ( myDataOut3 & (1 << i) ) {
      pinState = 1;
    }
    else {
      pinState = 0;
    }
    digitalWrite(myDataPin, pinState);
    digitalWrite(myClockPin, 1);
  }

  //End shifther
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);
  digitalWrite(latchPinLED, 1); //Disable data input
}

void displayDigit(byte character, int posInt) {

  byte pos = 0;

  //Enable data input
  digitalWrite(latchPin, LOW);

  shiftOut(dataPin, clockPin, MSBFIRST, character);
  bitWrite(pos, posInt, 1);
  shiftOut(dataPin, clockPin, MSBFIRST, pos);

  //Disable data input
  digitalWrite(latchPin, HIGH);
}

void setup() {

  Serial.begin(9600);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(latchPinLED, OUTPUT);
  pinMode(clockPinLED, OUTPUT);
  pinMode(dataPinLED, OUTPUT);

}

void loop() {

  if (Serial.available() > 0) {

    if (Serial.available() > 12) {

      //Clean array
      memset(trama, 0, sizeof(trama));

      trama[0] = Serial.read();
      // Check header
      if (trama[0] == 0xCA) {

        // Fill array
        for (int i = 1; i < 12; i++) {

          trama[i] = Serial.read();
        }
      }
    }
  }
  //Posiciones 0b45670123
  displayDigit(trama[1], 3);
  displayDigit(trama[2], 2);
  displayDigit(trama[3], 1);
  displayDigit(trama[4], 0);

  displayDigit(trama[5], 7);
  displayDigit(trama[6], 6);
  displayDigit(trama[7], 5);
  displayDigit(trama[8], 4);
  shiftOutLEDS(dataPinLED, clockPinLED, trama[11], trama[10], trama[9]);
}