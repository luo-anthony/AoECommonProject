int PIN_BTN = A1;
int PIN_BTN2 = A2;
int PIN_ANALOGX = A3;
int PIN_ANALOGY = A4;
int SHIFT_DATA = 11;
int SHIFT_CLK = 12;
int SHIFT_LATCH = 13;

int buttonReads[10];

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_BTN, INPUT);
  pinMode(PIN_ANALOGX, INPUT);
  pinMode(PIN_ANALOGY, INPUT);
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  Serial.begin(9600);
}

void registerWrite(byte writeByte) {
  digitalWrite(SHIFT_LATCH, LOW);
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, writeByte);
  digitalWrite(SHIFT_LATCH, HIGH);
}

void registerClear() {
  byte zero = 0;
  registerWrite(zero);
}

void registerSingle(int bitNum) {
  byte writeVal = 0;
  bitWrite(writeVal, bitNum, 1);
  registerWrite(writeVal);
}

void readButtons() { //Should debounce
  for (int bNum = 0; bNum < 5; bNum++) {
    registerSingle(bNum);
    delay(3);
    int readVal = digitalRead(PIN_BTN);
    if (readVal != buttonReads[bNum]) {
      delay(15);
      readVal = digitalRead(PIN_BTN);
    }
    buttonReads[bNum] = readVal;
    buttonReads[bNum + 5] = digitalRead(PIN_BTN2);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(digitalRead(10));
  readButtons();
  for (int i = 0; i < 10; i++) {
    Serial.print(String(buttonReads[i]) + "\t");
  }
  Serial.print(map(analogRead(PIN_ANALOGX),0,1024,-90,90));
  Serial.print("\t");
  Serial.print(map(analogRead(PIN_ANALOGY),0,1024,-90,90));
  Serial.print("\t");
  Serial.println("|");
  delay(1);
}
