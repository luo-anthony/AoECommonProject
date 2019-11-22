int PIN_BTN = 10;
int SHIFT_DATA = 11;
int SHIFT_CLK = 12;
int SHIFT_LATCH = 13;
int LIGHTS_DATA = 7;
int LIGHTS_CLK = 8;
int LIGHTS_LATCH = 9;
int LIGHTS_DELTA = -1 * (SHIFT_DATA - LIGHTS_DATA);
int LIGHTS_SHOWTIME = 0;

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

int[8] led_activeRow = {};
void lights_drawRow(int colPin) { //If flashing, add clears to 0b11111111 and 0b00000000
  pinMode(rowPin, OUTPUT);
  digitalWrite(rowPin, HIGH);
  for (int i = 0; i < 8; i++) {
    if (led_activeRow[i] == 1) {
      registerSingle(i, LIGHTS_DELTA);
      delay(LIGHTS_SHOWTIME);
    }
  }
  digitalWrite(rowPin, LOW);
  for (int i = 0; i < 8; i++) {
    if (led_activeRow[i] == 2) {
      registerSingleNot(i, LIGHTS_DELTA);
      delay(LIGHTS_SHOWTIME);
    }
  }
  pinMode(rowPin, INPUT);
}

int[6][8] led_fullBoard = {};
void lights_drawBoard() {
  for (int rNum = 0; rNum < 6; rNum++) {
    led_activeRow = led_fullBoard[rNum];
    lights_drawRow(rNum);
  }
}

void readButtons() { //Should debounce
  for (int bNum = 0; bNum < 8; bNum++) {
    registerSingle(bNum);
    int readVal = digitalRead(PIN_BTN);
    if (readVal != buttonReads[bNum]) {
      delay(15);
      readVal = digitalRead(PIN_BTN);
    }
    buttonReads[bNum] = readVal;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(digitalRead(10));
  readButtons();
  for (int i = 0; i < 8; i++) {
    Serial.print(String(buttonReads[i]) + "\t");
  }
  Serial.println("|");
  delay(1);
}
