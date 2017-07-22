/*
 Pines del Display
 1 -> DataPin
 2 -> ClockPin
 3 -> LatchPin
 4 -> GND
 5 -> VCC

*/

int dataPin = 2;
int latchPin = 3;
int clockPin = 4;

void displayDigit(byte character, byte pos){

      //Activar entrada de datos
      digitalWrite(latchPin, LOW);
      
      shiftOut(dataPin, clockPin, MSBFIRST, character);
      shiftOut(dataPin, clockPin, MSBFIRST, pos);

      //Aplicar
      digitalWrite(latchPin, HIGH);
}

void setup() {
/*
  0b11000000;  // caracter[ 0 ]
  0b11111001;  // caracter[ 1 ]
  0b10100100;  // caracter[ 2 ]
  0b10110000;  // caracter[ 3 ]
  0b10011001;  // caracter[ 4 ]
  0b10010010;  // caracter[ 5 ]
  0b10000010;  // caracter[ 6 ]
  0b11111000;  // caracter[ 7 ]
  0b10000000;  // caracter[ 8 ]
  0b10011000;  // caracter[ 9 ]
  0b10111111; // caracter[ - ]
  0b10001000; // caracter[ R ]
*/

    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
}

void loop() {

    //Escribimos un gion [ - ] en la posicion 1 -> 0b45670123
    displayDigit(0b10111111,0b00000010);        
    //Escribimos una gion [ R ] en la posicion 3 -> 0b45670123
    displayDigit(0b10001000, 0b00001000);
}

