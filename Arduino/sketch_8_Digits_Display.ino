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

byte trama[12];

void displayDigit(byte character, int posInt){

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

    Serial.begin(57600);
    
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
}

void loop() {

    if (Serial.available() > 0) {

      if(Serial.available() > 12) {

        //Clean array
        //memset(trama, 0, sizeof(trama));
        
        trama[0] = Serial.read();
        // Check header
        if (trama[0]==0xFF){

          // Fill array
          for (int i = 1; i < 12; i++){
            
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
}
