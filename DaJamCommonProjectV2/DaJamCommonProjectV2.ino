const int ROWS = 6;
const int COLUMNS = 8;
int arr[ROWS][COLUMNS] = {}; //the lighting state of the gameState
int gameStateArr[ROWS][COLUMNS] = {};
enum Move {reverse, rotate_left, rotate_right, add, nomove};
enum GameState {playing, P1_won, P2_won, tie};
enum JoystickMove {up, down, left, right, center};
int temprow[COLUMNS];
int tempcolumn[ROWS];
int player = 1; //this value must be updated at every time and can only be 0, 1, 2,: P1= 1, P2 = 2
GameState gameState = playing;
int horizontalDiagonalCheck[ROWS][COLUMNS * 3] = {};
int tempHorizontalRow[COLUMNS * 3];

int PIN_BTN1 = A1;
int PIN_BTN2 = A2;
int SHIFT_DATA = 11;
int SHIFT_CLK = 12;
int SHIFT_LATCH = 13;
int LIGHTS_DATA = 5;
int LIGHTS_CLK = 6;
int LIGHTS_LATCH = 7;
int LIGHTS_DELTA = -1 * (SHIFT_DATA - LIGHTS_DATA);
int LIGHTS_SHOWTIME = 200;
int LIGHTS_OE = SCL; //A4 or A5

int PIN_JOYSTICKX = A3;
int PIN_JOYSTICKY = A4;

int LIGHTS_ROWPINS[6] = {8, 2, 10, 4, 9, 3};

int buttonReads[10];
bool flipButton = false;
bool rotateButton = false;

bool redTurn = false;

//void ReverseLights(int column);
//void RotateLeft(int row);
//bool makeMove(Move m, int column, int row);
//void winSequence();
//void resetGame();
//void switchPlayer();
//void copyRow(int row);
//void copyColumn(int column);
//void ReverseLights(int column);
//void RotateLeft(int row);
//void RotateRight(int row);
//bool AddPiece(int column);
//int checkifArrayContainsFour(int temparr[]);
//void copyBigHorizontal(int row);
//void isGameWon();
//void parseInputs();
//void resetInputs();


void setup() {
  // put your setup code here, to run once:
  clearGameState();
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(PIN_BTN1, INPUT);
  pinMode(PIN_BTN2, INPUT);
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  pinMode(LIGHTS_DATA, OUTPUT);
  pinMode(LIGHTS_CLK, OUTPUT);
  pinMode(LIGHTS_LATCH, OUTPUT);

  pinMode(LIGHTS_OE, OUTPUT);
  digitalWrite(LIGHTS_OE, LOW);

  for (int rowPinIdx = 0; rowPinIdx < 6; rowPinIdx++) {
    pinMode(LIGHTS_ROWPINS[rowPinIdx], INPUT);
  }

  Serial.begin(9600);
}

//start frontend code

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
  if (delta == 0) { //For player turn indicator
    if (redTurn) {
      bitWrite(writeVal, 6, 1);
    }
    else {
      bitWrite(writeVal, 7, 1);
    }
  }
  registerWrite(writeVal, delta);
}

void registerSingleNot(int bitNum, int delta = 0) {
  byte writeVal = 0b11111111;
  bitWrite(writeVal, bitNum, 0);
  registerWrite(writeVal, delta);
}

int led_activeRow[8] = {0, 1, 0, 1, 0, 1, 0, 1};
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

void lights_drawBoard() {
  for (int rNum = 0; rNum < 6; rNum++) {
    memcpy(led_activeRow, arr[rNum], sizeof(arr[rNum][0]) * 8); //led_activeRow = led_fullBoard[rNum];
    lights_drawRow(LIGHTS_ROWPINS[rNum]);
  }
}

void readButtons() { //Should debounce
  for (int bNum = 0; bNum < 5; bNum++) {
    registerSingle(bNum);
    int readVal = digitalRead(PIN_BTN1);
    int readVal2 = digitalRead(PIN_BTN2);

    buttonReads[bNum] = readVal;
    buttonReads[bNum + 5] = readVal2;
  }
  flipButton = buttonReads[8];
  rotateButton = buttonReads[9];
}




int joystickX = 0;
int joystickY = 0;
void readJoystick() {
  joystickX = map(analogRead(PIN_JOYSTICKX), 0, 1024, -90, 90);
  joystickY = map(analogRead(PIN_JOYSTICKY), 0, 1024, -90, 90);
}

// ADD FUNCTION TO GET STATE OF TWO NEW BUTTONS

//start backend code

void copytoArrayFrom(int fromArr[ROWS][COLUMNS]) {
  for (int i = 0; i < COLUMNS; i += 1) {
    for (int j = 0; j < ROWS; j += 1) {
      arr[j][i] = fromArr[j][i];
    }
  }
}

void copytoGameArrayFrom(int fromArr[ROWS][COLUMNS]) {
  for (int i = 0; i < COLUMNS; i += 1) {
    for (int j = 0; j < ROWS; j += 1) {
      gameStateArr[j][i] = fromArr[j][i];
    }
  }
}

void clearGameState() {
  for (int i = 0; i < COLUMNS; i += 1) {
    for (int j = 0; j < ROWS; j += 1) {
      arr[j][i] = 0;
      gameStateArr[j][i] = 0;
    }
  }
}

Move nextMove = nomove;
int move_column = -1;
int move_row = -1;
void loop() {
  while (gameState == playing) {
    lights_drawBoard();
    bool validMove = false;
    while (!validMove) {
      lights_drawBoard();
      while (nextMove == nomove) {
        lights_drawBoard();
        parseInputs();
        validMove = makeMove(nextMove, move_column, move_row);
        if (!validMove) {
          resetInputs();
          nextMove = nomove;
        }
      }
      lights_drawBoard();
      isGameWon();
      resetInputs();
      switchPlayer();
    }
    winSequence();
    resetGame();
  }
}

bool joystickTriggered = false;
JoystickMove last_partialDirection = center;
int boardColumnPointer = 0;
int boardRowPointer = 0;

bool RotateActivated = false;
bool FlipActivated = false;
bool RotateButtonLastState = false;
bool FlipButtonLastState = false;
bool moveConfirmed = false;

void resetInputs() {
  move_row = -1;
  move_column = -1;
  joystickTriggered = false;
  last_partialDirection = center;
  nextMove = nomove;
  boardColumnPointer = 0;
  boardRowPointer = 0;
  RotateActivated = false;
  FlipActivated = false;
  RotateButtonLastState = false;
  FlipButtonLastState = false;
}


void parseInputs() {
  int column = -1;
  readButtons();
  for (int i = 0; i < 8; i ++) {
    if (buttonReads[i] == 1) {
      column = i;
      nextMove = add;
      break;
    }
  }
  if (!rotateButton) {
    RotateButtonLastState = false;
  }
  if (!flipButton) {
    FlipButtonLastState = false;
  }

  if (rotateButton && !RotateButtonLastState && !RotateActivated) {
    RotateActivated = true;
    RotateButtonLastState = true;
  }
  else if (flipButton && !FlipButtonLastState && !FlipActivated) {
    FlipActivated = true;
    FlipButtonLastState = true;
  }
  else if (rotateButton && !RotateButtonLastState && RotateActivated) {
    moveConfirmed = true;
  }
  else if (flipButton && !FlipButtonLastState && FlipActivated) {
    moveConfirmed = true;
  }

  if (column == -1) {
    if (RotateActivated && !moveConfirmed) {
      animateFlashRow(boardRowPointer);
      JoystickMove direction = parseJoystickInputs();
      if (direction == up) {
        boardRowPointer = boardRowPointer + 1 % 6;
      }
      else if (direction == down) {
        boardRowPointer--;
        if (boardRowPointer < 0) {
          boardRowPointer = 5;
        }
      }
      animateFlashRow(boardRowPointer);

    }
    else if (FlipActivated && !moveConfirmed) {
      animateFlashColumn(boardColumnPointer);
      JoystickMove direction = parseJoystickInputs();
      if (direction == right) {
        boardColumnPointer = boardColumnPointer + 1 % 8;
      }
      else if (direction == left) {
        boardColumnPointer--;
        if (boardColumnPointer < 0) {
          boardColumnPointer = 7;
        }
      }
      animateFlashColumn(boardColumnPointer);
    }
    else if (RotateActivated && moveConfirmed) {
      JoystickMove direction = parseJoystickInputs();
      if (direction == right) {
        nextMove = rotate_right;
        move_row = boardRowPointer;
      }
      else if (direction == left) {
        nextMove = rotate_left;
        move_row = boardRowPointer;
      }
    }
    else if (FlipActivated && moveConfirmed) {
      nextMove = reverse;
      move_column = boardColumnPointer;
    }
  }
  delayAndLight(1);
}

JoystickMove parseJoystickInputs() {
  readJoystick();
  if (!joystickTriggered) {
    if (joystickX > 40) {
      joystickTriggered = true;
      last_partialDirection = right;
    }
    else if (joystickX < -40) {
      joystickTriggered = true;
      last_partialDirection = left;
    }
    else if (joystickY > 40) {
      joystickTriggered = true;
      last_partialDirection = up;
    }
    else if (joystickY < -40) {
      joystickTriggered = true;
      last_partialDirection = down;
    }
  }
  else if (joystickY < 15 && joystickY > -15 && joystickX < 15 && joystickX > -15) {
    JoystickMove output = last_partialDirection;
    joystickTriggered = false;
    last_partialDirection = center;
    return output;
  }
  return center;
}

void animateFlashRow(int row) {
  copytoGameArrayFrom(arr); //create backup of the game state
  int numFlashes = 3;
  for (int j = 0; j < numFlashes; j++) {
    for (int i = 0; i < 8; i ++) {
      arr[row][i] = player;
    }
    //must keep parsing inputs 
    delayAndLight(150); //to be adjusted
    for (int i = 0; i < 8; i ++) {
      arr[row][i] = 0;
    }
    delayAndLight(100);
  }
  copytoArrayFrom(gameStateArr); //resets the arr to one with pieces
}

void animateFlashColumn(int column) {
  copytoGameArrayFrom(arr); //create backup of the game state
  int numFlashes = 3;
  for (int j = 0; j < numFlashes; j++) {
    for (int i = 0; i < 6; i ++) {
      arr[i][column] = player;
    }
    delayAndLight(150); //to be adjusted
    for (int i = 0; i < 6; i ++) {
      arr[i][column] = 0;
    }
    delayAndLight(100);
  }
  copytoArrayFrom(gameStateArr); //resets the arr to one with pieces

}

void delayAndLight(int time) { //time in milliseconds
  for (int i = 0; i < time; i++) {
    lights_drawBoard();
  }
}

bool makeMove(Move m, int column, int row) {
  bool validMove = true;
  switch (m) {
    case reverse:
      ReverseLights(column);
      break;
    case rotate_left:
      RotateLeft(row);
      break;
    case rotate_right:
      RotateRight(row);
      break;
    case add:
      validMove = AddPiece(column);
      break;
  }
  return validMove;
}

void winSequence() {
 //TODO - David code some fancy flash animation thanks
}

void resetGame() {
  clearGameState();
  int player = 1;
  redTurn = true;
  gameState = playing;
  delayAndLight(100);
}

void switchPlayer() {
  if (player == 1) {
    player = 2;
    redTurn = false;
  }
  else {
    player = 1;
    redTurn = true;
  }
  delayAndLight(5);
}

void copyRow(int row) {
  for (int i = 0; i < COLUMNS; i ++) {
    temprow[i] = arr[row][i];
  }
}

void copyColumn(int column) {
  for (int i = 0; i < ROWS; i ++) {
    tempcolumn[i] = arr[i][column];
  }
}

int countOccurences(int array[], int num) { //test this function
  int count = 0;
  int n = sizeof(array) / sizeof(array[0]);
  for (int i = 0; i < n; i++) {
    if (arr[i == num]) {
      count++;
    }
  }
  return count;
}
void ReverseLights(int column) {
  delayAndLight(50);
  copyColumn(column);
  int numzeros = countOccurences(tempcolumn, 0);
  int onetwos[ROWS - numzeros];
  for (int i = numzeros; i < ROWS; i++) {
    onetwos[i - numzeros] = tempcolumn[i];
  }
  for (int i = 0; i < ROWS; i ++) {
    arr[i][column] = 0;
  }
  //animate falling
  for (int i = 0; i < numzeros; i ++) {
    for (int j = 0; j < ROWS - numzeros; j ++) {
      arr[i][column] = onetwos[j];
    }
    delayAndLight(500);
    for (int i = 0; i < ROWS; i ++) {
      arr[i][column] = 0;
    }
  }
  for (int i = 0; i < ROWS - numzeros; i ++) {
    arr[ROWS - 1 - i][column] = onetwos[i];
  }
  delayAndLight(1000);
  //NEED TO CHECK IF THIS WORKS LOL
}


void RotateLeft(int row) {
  delayAndLight(50);
  copyRow(row);
  for (int i = 0; i < 7; i ++) {
    arr[row][i] = temprow[i + 1];
  }
  arr[row][COLUMNS - 1] = temprow[0];
  delayAndLight(500);
}

void RotateRight(int row) {
  delayAndLight(50);
  copyRow(row);
  for (int i = 1; i < COLUMNS; i ++) {
    arr[row][i] = temprow[i - 1];
  }
  arr[row][0] = temprow[COLUMNS - 1];
  delayAndLight(500);
}


/*
  Returns true if valid move, false if the column is full already
*/
bool AddPiece(int column) {
  delayAndLight(200);
  int piecePosition = -1;
  for (int i = 5; i >= 0; i --) {
    delayAndLight(1);
    if (arr[i][column] == 0) {
      if (i == -1) {
        return false;
      }
      gameStateArr[i][column] = player;
      piecePosition = i;
      break;
    }
  }
  for (int i = 0; i > piecePosition; i--) {
    arr[i][column] = player;
    delayAndLight(500);
    arr[i][column] = 0;
    delayAndLight(10);
  }
  copytoArrayFrom(gameStateArr);
  delayAndLight(1000);
  return true;
}

int checkifArrayContainsFour(int temparr[]) {
  int output = 0;
  int n = sizeof(temparr) / sizeof(temparr[0]);
  for (int i = 0; i < n - 3; i ++) {
    if ((temparr[i] == temparr[i + 1]) && (temparr[i] == temparr[i + 2]) && (temparr[i] == temparr[i + 3]) && (temparr[i] == temparr[i + 4])) { //yes this is stupid code but whatever I don't want to use standard library lol
      output = temparr[i];
      break;
    }
  }
  return output;
}

void copyBigHorizontal(int row) {
  for (int i = 0; i < 3 * COLUMNS; i ++) {
    tempHorizontalRow[i] = horizontalDiagonalCheck[row][i];
  }
}

void isGameWon() {
  delayAndLight(10);
  //checking vertical
  bool PlayerOneWon = false;
  bool PlayerTwoWon = false;
  for (int i = 0; i < COLUMNS; i ++) {
    copyColumn(i);
    if (checkifArrayContainsFour(tempcolumn) == 1) { //change
      PlayerOneWon = true;
    }
    if (checkifArrayContainsFour(tempcolumn) == 2) { //change
      PlayerTwoWon = true;
    }
  }
  delayAndLight(2);

  //checking horizontal

  //copy arr into triple state
  for (int i = 0; i < COLUMNS * 3; i += COLUMNS) {
    for (int j = 0; j < ROWS; j += 1) {
      for (int k = 0; k < COLUMNS; k += 1) {
        horizontalDiagonalCheck[j][i + k] = arr[j][k];
      }
    }
  }
  delayAndLight(2);

  //check each row
  for (int i = 0; i < ROWS; i++) {
    copyBigHorizontal(i);
    for (int j = 0; j < (COLUMNS * 3) - 3; j++) {
      int checkfour[4] = {};
      for (int k = 0; k < 4; k ++) {
        checkfour[k] = tempHorizontalRow[j + k];
      }
      int result = checkifArrayContainsFour(checkfour);
      if (result == 1) {
        PlayerOneWon = true;
      }
      if (result == 2) {
        PlayerTwoWon = true;
      }

    }
  }
  delayAndLight(2);

  //check diagonal
  for (int i = 0; i < (COLUMNS * 3) - 3; i++) {
    for (int j = 0; j < ROWS - 3; j++) {
      int checkfour[4] = {};
      for (int k = 0; k < 4; k ++) {
        checkfour[k] = horizontalDiagonalCheck[j + k][i + k];
      }
      int result = checkifArrayContainsFour(checkfour);
      if (result == 1) {
        PlayerOneWon = true;
      }
      if (result == 2) {
        PlayerTwoWon = true;
      }
    }
  }
  delayAndLight(2);


  for (int i = 3; i < (COLUMNS * 3); i++) {
    for (int j = 3; j < ROWS; j++) {
      int checkfour[4] = {};
      for (int k = 0; k < 4; k ++) {
        checkfour[k] = horizontalDiagonalCheck[j - k][i - k];
      }
      int result = checkifArrayContainsFour(checkfour);
      if (result == 1) {
        PlayerOneWon = true;
      }
      if (result == 2) {
        PlayerTwoWon = true;
      }
    }
  }
  delayAndLight(2);


  if (PlayerOneWon == true && PlayerTwoWon == true) {
    gameState = tie;
  }
  if (PlayerOneWon == true) {
    gameState = P1_won;
  }
  if (PlayerTwoWon == true) {
    gameState = P2_won;
  }
  delayAndLight(5);
}
