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

int dataPinLED = 8;
int latchPinLED = 9;
int clockPinLED = 10;

byte trama[12];

// My shitfOut
void shiftOutLEDS(int myDataPin, int myClockPin, byte myDataOut) {

  //Setup inicial
  int i = 0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  for (i = 0; i <= 7; i++)  {
    digitalWrite(myClockPin, 0);

    if ( myDataOut & (1 << i) ) {
      pinState = 1;
    }
    else {
      pinState = 0;
    }

    digitalWrite(myDataPin, pinState);
    digitalWrite(myClockPin, 1);
    digitalWrite(myDataPin, 0);
  }

  //Fin shifther
  digitalWrite(myClockPin, 0);
}

void displayDigit(byte character, int posInt) {

  byte pos = 0;

  //Activar entrada de datos
  digitalWrite(latchPin, LOW);

  shiftOut(dataPin, clockPin, MSBFIRST, character);
  bitWrite(pos, posInt, 1);
  shiftOut(dataPin, clockPin, MSBFIRST, pos);

  //Cerrar entrada de datos
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
      if (trama[0] == 0xFF) {

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

  //Activar entrada de datos
  digitalWrite(latchPinLED, 0);

  shiftOutLEDS(dataPinLED, clockPinLED, trama[10]);
  shiftOutLEDS(dataPinLED, clockPinLED, trama[9]);

  //Desactivar entrada de datos
  digitalWrite(latchPinLED, 1);

}
