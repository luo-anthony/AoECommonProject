int PIN_BTN = A0;
int SHIFT_DATA = 11;
int SHIFT_CLK = 12;
int SHIFT_LATCH = 13;
int LIGHTS_DATA = 5;
int LIGHTS_CLK = 6;
int LIGHTS_LATCH = 7;
int LIGHTS_DELTA = -1 * (SHIFT_DATA - LIGHTS_DATA);
int LIGHTS_SHOWTIME = 200;
int LIGHTS_OE = SCL; //A4 or A5

int PIN_JOYSTICKX = A1;
int PIN_JOYSTICKY = A2;

int LIGHTS_ROWPINS[6] = {8,2,10,4,9,3};

int buttonReads[8];

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_BTN, INPUT);
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  pinMode(LIGHTS_DATA, OUTPUT);
  pinMode(LIGHTS_CLK, OUTPUT);
  pinMode(LIGHTS_LATCH, OUTPUT);

  pinMode(LIGHTS_OE, OUTPUT);
  digitalWrite(LIGHTS_OE, LOW);

  for (int rowPinIdx=0; rowPinIdx<6; rowPinIdx++) {
    pinMode(LIGHTS_ROWPINS[rowPinIdx], INPUT);
  }
  
  Serial.begin(9600);
}

void registerWrite(byte writeByte, int delta = 0) {
  digitalWrite(SHIFT_LATCH + delta, LOW);
  shiftOut(SHIFT_DATA + delta, SHIFT_CLK + delta, MSBFIRST, writeByte);
  digitalWrite(SHIFT_LATCH + delta, HIGH);
}

void registerClear(int delta = 0) {
  byte zero = 0;
  registerWrite(zero, delta);
}

void registerClearNot(int delta = 0) {
  byte one = 0b11111111;
  registerWrite(one, delta);
}

void registerSingle(int bitNum, int delta = 0) {
  byte writeVal = 0;
  bitWrite(writeVal, bitNum, 1);
  registerWrite(writeVal, delta);
}

void registerSingleNot(int bitNum, int delta = 0) {
  byte writeVal = 0b11111111;
  bitWrite(writeVal, bitNum, 0);
  registerWrite(writeVal, delta);
}

int led_activeRow[8] = {0,1,0,1,0,1,0,1};
void lights_drawRow(int rowPin) { //If flashing, add clears to 0b11111111 and 0b00000000
  digitalWrite(LIGHTS_OE, HIGH);
  //pinMode(rowPin, INPUT);
  registerClearNot(LIGHTS_DELTA);
  pinMode(rowPin, OUTPUT);
  digitalWrite(rowPin, HIGH);
  digitalWrite(LIGHTS_OE, LOW);
  for (int i = 0; i < 8; i++) {
    if (led_activeRow[i] == 1) {
      registerSingleNot(i, LIGHTS_DELTA);
      delayMicroseconds(LIGHTS_SHOWTIME);
    }
  }
  //pinMode(rowPin, INPUT);
  digitalWrite(LIGHTS_OE, HIGH);
  registerClear(LIGHTS_DELTA);
  //pinMode(rowPin, OUTPUT);
  digitalWrite(rowPin, LOW);
  digitalWrite(LIGHTS_OE, LOW);
  for (int i = 0; i < 8; i++) {
    if (led_activeRow[i] == 2) {
      registerSingle(i, LIGHTS_DELTA);
      delayMicroseconds(LIGHTS_SHOWTIME);
    }
  }
  digitalWrite(rowPin, HIGH);
  pinMode(rowPin, INPUT);
}

int arr[6][8] = {{0,1,0,0,0,0,0,0},
                           {0,0,0,0,0,0,0,0},
                           {0,0,0,0,0,0,0,0},
                           {0,0,0,0,2,0,0,0},
                           {0,0,0,0,0,0,0,0},
                           {1,2,1,2,1,2,1,2}};

/*int arr[6][8] = {{1,1,1,1,1,1,1,1},
                  {1,1,1,1,1,1,1,1},
                  {1,1,1,1,1,1,1,1},
                  {1,1,1,1,1,1,1,1},
                  {1,1,1,1,1,1,1,1},
                  {1,1,1,1,1,1,1,1}};*/

/*
 * int arr[6][8] = {{2,2,2,2,2,2,2,2},
                  {2,2,2,2,2,2,2,2},
                  {2,2,2,2,2,2,2,2},
                  {2,2,2,2,2,2,2,2},
                  {2,2,2,2,2,2,2,2},
                  {2,2,2,2,2,2,2,2}};
 */

void lights_drawBoard() {
  for (int rNum = 0; rNum < 6; rNum++) {
    memcpy(led_activeRow, arr[rNum], sizeof(arr[rNum][0])*8); //led_activeRow = led_fullBoard[rNum];
    lights_drawRow(LIGHTS_ROWPINS[rNum]);
  }
}

void readButtons() { //Should debounce
  for (int bNum = 0; bNum < 8; bNum++) {
    registerSingle(bNum);
    int readVal = digitalRead(PIN_BTN);
    if (readVal != buttonReads[bNum]) {
      delay(15); //SHOULD FIX THIS
      readVal = digitalRead(PIN_BTN);
    }
    buttonReads[bNum] = readVal;
  }
}

int joystickX = 0;
int joystickY = 0;
void readJoystick() {
  joystickX = map(analogRead(PIN_JOYSTICKX), 0, 1024, -50, 50);
  joystickY = map(analogRead(PIN_JOYSTICKY), 0, 1024, -50, 50);
}

void resetBoard() {
  for (int yIdx = 0; yIdx < 6; yIdx++) {
    for (int xIdx = 0; xIdx < 8; xIdx++) {
      arr[yIdx][xIdx] = 0;
    }
  }
}

int clampRegion(int input, int maxVal) { //maxVal is actually not allowed, it equals zero
  if (input >= maxVal) return input - maxVal;
  if (input < 0) return input + maxVal;
  return input;
}


  int counterx = 0;
void loop() {
  /*lights_drawBoard();
  delayMicroseconds(2);
  return;*/
  // put your main code here, to run repeatedly:
  //Serial.println(digitalRead(10));
  /*readButtons();
  for (int i = 0; i < 8; i++) {
    Serial.print(String(buttonReads[i]) + "\t");
  }
  Serial.println("|");
  delay(1);*/

  //for (int counter=0;counter<6;counter++) {
  
  /*pinMode(LIGHTS_ROWPINS[0], OUTPUT); //SINGLE LED MANUAL
  digitalWrite(0, LOW);
  registerSingle(2, LIGHTS_DELTA);
  return;*/
    /* Spiral Animation
     * for (int rep=0; rep<1; rep++) {
      for (int rep2=0; rep2<8; rep2++) {
          led_activeRow[0] = 0;
          led_activeRow[1] = 0;
          led_activeRow[2] = 0;
          led_activeRow[3] = 0;
          led_activeRow[4] = 0;
          led_activeRow[5] = 0;
          led_activeRow[6] = 0;
          led_activeRow[7] = 0;
          led_activeRow[rep2] = 1;
          int tOut = rep2 + 4;
          if (tOut > 7) tOut -= 8;
          led_activeRow[tOut] = 2;
          for (int r3=0;r3<300;r3++) {
            //lights_drawBoard();
            lights_drawRow(LIGHTS_ROWPINS[counterx]);
          }
          Serial.print(counterx);
          Serial.print("\t");
          counterx += 1;
          if (counterx == 6) counterx = 0;

          Serial.println(counterx);
      }
    //}*/

    //resetBoard();
    counterx += 1;
    if (counterx >= 8) counterx = 0;
    for (int colIdx = 0; colIdx < 1; colIdx++) {
      resetBoard();
      arr[clampRegion(colIdx + 0, 6)][clampRegion(counterx + 0, 8)] = 1;
      arr[clampRegion(colIdx + 0, 6)][clampRegion(counterx + 4, 8)] = 2;
      arr[clampRegion(colIdx + 1, 6)][clampRegion(counterx + 1, 8)] = 1;
      arr[clampRegion(colIdx + 1, 6)][clampRegion(counterx + 5, 8)] = 2;
      arr[clampRegion(colIdx + 2, 6)][clampRegion(counterx + 2, 8)] = 1;
      arr[clampRegion(colIdx + 2, 6)][clampRegion(counterx + 6, 8)] = 2;
      arr[clampRegion(colIdx + 3, 6)][clampRegion(counterx + 3, 8)] = 1;
      arr[clampRegion(colIdx + 3, 6)][clampRegion(counterx + 7, 8)] = 2;
      arr[clampRegion(colIdx + 4, 6)][clampRegion(counterx + 4, 8)] = 1;
      arr[clampRegion(colIdx + 4, 6)][clampRegion(counterx + 8, 8)] = 2;
      arr[clampRegion(colIdx + 5, 6)][clampRegion(counterx + 5, 8)] = 1;
      arr[clampRegion(colIdx + 5, 6)][clampRegion(counterx + 9 - 8, 8)] = 2;
      for (int reps=0; reps<20; reps++) {
        lights_drawBoard();
      }
    }
    delayMicroseconds(1);
    //delay(50000);
  //}
}
