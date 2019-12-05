const int ROWS = 6;
const int COLUMNS = 8;
int arr[ROWS][COLUMNS] = {}; //the lighting state of the gameState
int gameStateArr[ROWS][COLUMNS] = {};
enum Move {reverse, rotate_left, rotate_right, add, nomove};
enum GameState {playing, P1_won, P2_won, tie};
enum JoystickMove {center, up, down, left, right};
enum Action {flashRow, flashColumn, noFlash, Drop};
int temprow[COLUMNS] = {};
int tempcolumn[ROWS] = {};
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

int LIGHTS_ROWPINS[6] = {4, 9, 3, 8, 2, 10};

int buttonReads[10];
bool flipButton = false;
bool rotateButton = false;

bool redTurn = false;



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
  digitalWrite(LIGHTS_OE, HIGH);
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
  joystickX = map(analogRead(PIN_JOYSTICKX), 0, 1024, -100, 100);
  joystickY = map(analogRead(PIN_JOYSTICKY), 0, 1024, -100, 100);
}

// ADD FUNCTION TO GET STATE OF TWO NEW BUTTONS

void DEBUG() {
  Serial.println("CURRENT LIGHTING ARRAY");
  for (int j = 0; j < ROWS; j += 1) {
    String s = "";
    for (int i = 0; i < COLUMNS; i += 1) {
      s = s + String(arr[j][i]) + ",";
    }
    Serial.println(s);
  }
  Serial.println("\n");
}


void GAMEDEBUG() {
  Serial.println("CURRENT GAMESTATE ARRAY");
  for (int j = 0; j < ROWS; j += 1) {
    String s = "";
    for (int i = 0; i < COLUMNS; i += 1) {
      s = s + String(gameStateArr[j][i]) + ",";
    }
    Serial.println(s);
  }
  Serial.println("\n");
}

void PRINTARR(int a[], int sizes, String names) {
  String s = "";
  for (int i = 0; i < sizes; i ++) {
    s = s + String(a[i]) + ",";
  }
  Serial.println(names + ":  " + s);
}

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

void TESTcopyArrayState(int fromArr[ROWS][COLUMNS]) {
  for (int i = 0; i < COLUMNS; i += 1) {
    for (int j = 0; j < ROWS; j += 1) {
      arr[j][i] = fromArr[j][i];
    }
  }
}

//debug only
//
//int TESTARRAY[6][8] =
//{ {0 , 1 , 2, 0 , 0 , 0 , 0, 0},
//  {2 , 1 , 1, 0 , 0 , 0 , 0, 0},
//  {1 , 2 , 1, 2 , 0 , 0 , 0, 0},
//  {2 , 1 , 2, 3 , 0 , 0 , 0, 0},
//  {1 , 2 , 3, 3 , 0 , 0 , 0, 0},
//  {2 , 3 , 3, 3 , 0 , 0 , 0, 0 }
//};

bool RotateActivated = false;
bool FlipActivated = false;
Move nextMove = nomove;
int move_column = -1;
int move_row = -1;

void loop() {
//  TESTcopyArrayState(TESTARRAY);
  DEBUG();
  while (gameState == playing) {
    lights_drawBoard();
    bool validMove = false;
    while (!validMove) {
      lights_drawBoard();
      while (nextMove == nomove) {
        lights_drawBoard();
        Action a = parseInputs();
        bool okay = true;
        while (RotateActivated && okay && nextMove == nomove) {
          okay = animateFlashRow();
          parseInputs();
        }
        while (FlipActivated && okay && nextMove == nomove) {
          okay = animateFlashColumn();
          parseInputs();
        }
        if (nextMove != nomove) {
          validMove = makeMove(nextMove, move_column, move_row);
          Serial.println("Move made: " + String(nextMove));
          Serial.println("Move_Column: " + String(move_column) + " | Move_Row: " + String(move_row));
        }
        if (!validMove) {
          resetInputs();
          nextMove = nomove;
        }
      }
    }
    DEBUG();
    Serial.println("finishedMove");
    lights_drawBoard();
    isGameWon();
    if (gameState == playing) {
      resetInputs();
      switchPlayer();
    }
  }
  delayAndLight(500);
  winSequence();
  delayAndLight(7000);
  resetGame();
}

bool joystickTriggered = false;
JoystickMove last_partialDirection = center;
int boardColumnPointer = 0;
int boardRowPointer = 0;



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
  moveConfirmed = false;
  RotateButtonLastState = false;
  FlipButtonLastState = false;
}


Action parseInputs() {
  int column = -1;
  readButtons();
  for (int i = 0; i < 8; i ++) {
    if (buttonReads[i] == 1) {
      column = i;
      move_column = i;
      nextMove = add;
      Serial.println("Column Button Pressed: " + String(i));
      return Drop;
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
    Serial.println("Rotate Button Activated");
    return flashRow;
  }
  else if (flipButton && !FlipButtonLastState && !FlipActivated) {
    FlipActivated = true;
    FlipButtonLastState = true;
    Serial.println("Flip Button Activated");
    return flashColumn;
  }
  else if (rotateButton && !RotateButtonLastState && RotateActivated) {
    Serial.println("Rotate Confirmed");
    moveConfirmed = true;
  }
  else if (flipButton && !FlipButtonLastState && FlipActivated) {
    Serial.println("Flip Confirmed");
    moveConfirmed = true;
  }

  if (column == -1) {
    if (RotateActivated && !moveConfirmed) {
      JoystickMove direction = parseJoystickInputs();
      //Serial.println("Joystick Direction: " + String(direction));
      if (direction == up) {
        boardRowPointer = (boardRowPointer + 1) % 6;
      }
      else if (direction == down) {
        boardRowPointer--;
        if (boardRowPointer < 0) {
          boardRowPointer = 5;
        }
      }
      return flashRow;
    }
    else if (FlipActivated && !moveConfirmed) {
      JoystickMove direction = parseJoystickInputs();
      //Serial.println("Joystick Direction: " + String(direction));

      if (direction == right) {
        boardColumnPointer = (boardColumnPointer + 1) % 8;
      }
      else if (direction == left) {
        boardColumnPointer--;
        if (boardColumnPointer < 0) {
          boardColumnPointer = 7;
        }
      }
      return flashColumn;
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
  return noFlash;
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

bool animateFlashRow() {
  int row = boardRowPointer;
  //Serial.println("Flashing Row: " + String(row));
  copytoGameArrayFrom(arr); //create backup of the game state
  //GAMEDEBUG();
  int numFlashes = 1;
  for (int j = 0; j < numFlashes; j++) {
    for (int i = 0; i < 8; i ++) {
      arr[row][i] = player;
    }
    for (int l = 0; l < 10; l++) {
      delayAndLight(10);
      Action a = parseInputs();
      if (a == Drop) {
        copytoArrayFrom(gameStateArr); //resets the arr to one with pieces
        return false;
      }
    }
    for (int i = 0; i < 8; i ++) {
      arr[row][i] = 0;
    }
    for (int l = 0; l < 5; l++) {
      delayAndLight(10);
      Action a = parseInputs();
      if (a == Drop) {
        copytoArrayFrom(gameStateArr); //resets the arr to one with pieces
        return false;
      }
    }
  }
  copytoArrayFrom(gameStateArr); //resets the arr to one with pieces
  return true;
}

bool animateFlashColumn() {
  int column = boardColumnPointer;
  //Serial.println("Flashing Column: " + String(column));
  copytoGameArrayFrom(arr); //create backup of the game state
  //GAMEDEBUG();
  int numFlashes = 1;
  for (int j = 0; j < numFlashes; j++) {
    for (int i = 0; i < 6; i ++) {
      arr[i][column] = player;
    }
    for (int l = 0; l < 10; l++) {
      delayAndLight(10);
      Action a = parseInputs();
      if (a == Drop) {
        copytoArrayFrom(gameStateArr); //resets the arr to one with pieces
        return false;
      }
    }
    for (int i = 0; i < 6; i ++) {
      arr[i][column] = 0;
    }
    for (int l = 0; l < 5; l++) {
      delayAndLight(10);
      Action a = parseInputs();
      if (a == Drop) {
        copytoArrayFrom(gameStateArr); //resets the arr to one with pieces
        return false;
      }
    }
  }
  copytoArrayFrom(gameStateArr); //resets the arr to one with pieces
  return true;
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
      addGravity();

      break;
    case rotate_left:
      RotateLeft(row);
      addGravity();
      break;
    case rotate_right:
      RotateRight(row);
      addGravity();
      break;
    case add:
      validMove = AddPiece(column);
      addGravity();
      break;
  }
  return validMove;
}

int clampRegion(int input, int maxVal) { //maxVal is actually not allowed, it equals zero
  if (input >= maxVal) return input - maxVal;
  if (input < 0) return input + maxVal;
  return input;
}

void resetBoard() {
  for (int yIdx = 0; yIdx < 6; yIdx++) {
    for (int xIdx = 0; xIdx < 8; xIdx++) {
      arr[yIdx][xIdx] = 0;
    }
  }
}

int counterx = 0;
void winSequence() {
  copytoGameArrayFrom(arr);
  Serial.println("Yay Player " + String(player) + " WON");
  if (gameState == tie) {
    for (int repNum = 0; repNum < 30; repNum++) {
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
        for (int reps = 0; reps < 20; reps++) {
          lights_drawBoard();
        }
      }
      delayMicroseconds(1);
    }
  }
  else {
    for (int repNum = 0; repNum < 30; repNum++) {
      counterx += 1;
      if (counterx >= 8) counterx = 0;
      for (int colIdx = 0; colIdx < 1; colIdx++) {
        resetBoard();
        arr[clampRegion(colIdx + 0, 6)][clampRegion(counterx + 0, 8)] = player;
        arr[clampRegion(colIdx + 0, 6)][clampRegion(counterx + 4, 8)] = player;
        arr[clampRegion(colIdx + 1, 6)][clampRegion(counterx + 1, 8)] = player;
        arr[clampRegion(colIdx + 1, 6)][clampRegion(counterx + 5, 8)] = player;
        arr[clampRegion(colIdx + 2, 6)][clampRegion(counterx + 2, 8)] = player;
        arr[clampRegion(colIdx + 2, 6)][clampRegion(counterx + 6, 8)] = player;
        arr[clampRegion(colIdx + 3, 6)][clampRegion(counterx + 3, 8)] = player;
        arr[clampRegion(colIdx + 3, 6)][clampRegion(counterx + 7, 8)] = player;
        arr[clampRegion(colIdx + 4, 6)][clampRegion(counterx + 4, 8)] = player;
        arr[clampRegion(colIdx + 4, 6)][clampRegion(counterx + 8, 8)] = player;
        arr[clampRegion(colIdx + 5, 6)][clampRegion(counterx + 5, 8)] = player;
        arr[clampRegion(colIdx + 5, 6)][clampRegion(counterx + 9 - 8, 8)] = player;
        for (int reps = 0; reps < 20; reps++) {
          lights_drawBoard();
        }
      }
      delayMicroseconds(1);
    }
  }
  copytoArrayFrom(gameStateArr);
}

void resetGame() {
  Serial.println("Resetting Game");
  clearGameState();
  int player = 1;
  redTurn = true;
  gameState = playing;
  delayAndLight(10);
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
  Serial.println("Switching Player | New Player:" + String(player));
  delayAndLight(5);
}

void copyRow(int row) {
  for (int i = 0; i < COLUMNS; i ++) {
    temprow[i] = arr[row][i];
  }
}

void copyColumn(int column) {
  for (int i = 0; i < ROWS; i ++) {
    tempcolumn[i] = int(arr[i][column]);
  }
}

int countOccurences(int array[], int num) { //test this function
  int count = 0;
  int n = sizeof(array) / sizeof(array[0]);
  for (int i = 0; i < n; i++) {
    if (int(arr[i]) == num) {
      count++;
    }
  }
  return count;
}
void ReverseLights(int column) {
  copytoGameArrayFrom(arr);
  Serial.println("Reversing Lights | Column: " + String(column));
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

  Serial.println("Start Reverse Falling Animation");
  //animate falling
  for (int i = 0; i < numzeros; i ++) {
    for (int j = 0; j < ROWS - numzeros; j ++) {
      arr[i][column] = onetwos[j];
    }
    delayAndLight(5);
    for (int i = 0; i < ROWS; i ++) {
      arr[i][column] = 0;
    }
  }
  for (int i = 0; i < ROWS - numzeros; i ++) {
    arr[ROWS - 1 - i][column] = onetwos[i];
  }
  Serial.println("Reverse Finished");
  delayAndLight(5);
  DEBUG();
  //NEED TO CHECK IF THIS WORKS LOL
}


void RotateLeft(int row) {
  Serial.println("Rotating Left | Row: " + String(row));

  delayAndLight(50);
  copyRow(row);
  for (int i = 0; i < 7; i ++) {
    arr[row][i] = temprow[i + 1];
  }
  arr[row][COLUMNS - 1] = temprow[0];
  delayAndLight(5);
  copytoGameArrayFrom(arr);
}

void RotateRight(int row) {
  Serial.println("Rotating Right | Row: " + String(row));
  delayAndLight(50);
  copyRow(row);
  PRINTARR(temprow, 8, "Temp Row:");
  for (int i = 1; i < COLUMNS; i ++) {
    arr[row][i] = temprow[i - 1];
  }
  arr[row][0] = temprow[COLUMNS - 1];
  delayAndLight(5);
  copytoGameArrayFrom(arr);
}

void addGravity() {
  for (int iNum = 0; iNum < ROWS; iNum++) {
    bool hasChanged = false;
    for (int rIdx = (ROWS - 1); rIdx > 0; rIdx--) { //Don't have to do gravity on top row
      for (int cIdx = 0; cIdx < COLUMNS; cIdx++) {
        if (arr[rIdx][cIdx] == 0) {
          for (int dIdx = rIdx; dIdx > 0; dIdx--) {
            arr[dIdx][cIdx] = arr[dIdx - 1][cIdx];
            if (arr[dIdx - 1][cIdx] != 0) hasChanged = true;
          }
          arr[0][cIdx] = 0; //Fill empty space on top
        }
      }
    }
    if (!hasChanged) break;
    else {
      delayAndLight(150);
    }
  }
}


/*
  Returns true if valid move, false if the column is full already
*/
bool AddPiece(int column) {
  Serial.println("Adding Piece Column: " + String(column));
  copytoGameArrayFrom(arr);
  delayAndLight(100);
  int piecePosition = -1;
  for (int i = 5; i >= 0; i --) {
    delayAndLight(1);
    if (arr[i][column] == 0) {
      gameStateArr[i][column] = player;
      piecePosition = i;
      break;
    }
  }
  Serial.println("Next Piece Location |  Column: " + String(column) + "  Row: " + String(piecePosition));
  if (piecePosition == -1) {
    Serial.println("Invalid Move");
    return false;
  }
  arr[0][column] = player;
  Serial.println("Start Fall Animation");
  //  for (int i = 0; i > piecePosition; i--) {
  //    DEBUG();
  //    arr[i][column] = player;
  //    delayAndLight(1);
  //    arr[i][column] = 0;
  //  }
  //copytoArrayFrom(gameStateArr);
  delayAndLight(10);
  Serial.println("Finish Adding Piece");
  DEBUG();
  delayAndLight(5);
  return true;
}

int checkifArrayContainsFour(int temparr[], int sizes) {
  //PRINTARR(tempcolumn, 6, "CheckColumn");
  int output = 0;
  for (int i = 0; i < sizes - 3; i ++) {
    int one = int(temparr[i]);
    if (one == int(temparr[i + 1]) && one == int(temparr[i + 2]) && one == int(temparr[i + 3]) && one > 0) { //yes this is stupid code but whatever I don't want to use standard library lol
      output = int(temparr[i]);
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

bool isBoardFull() {
  for (int i = 0; i < COLUMNS; i += 1) {
    for (int j = 0; j < ROWS; j += 1) {
      if (arr[j][i] == 0) {
        return false;
      }
    }
  }
  return true;
}

void isGameWon() {
  Serial.println("Running isGameWon()");
  delayAndLight(10);
  //checking vertical
  bool PlayerOneWon = false;
  bool PlayerTwoWon = false;
  Serial.println("Check Vertical Win");
  for (int i = 0; i < COLUMNS; i ++) {
    copyColumn(i);
    //Serial.println("Check Column: " + String(i));
    //PRINTARR(tempcolumn, 6,  "Column Contents");
    int check = checkifArrayContainsFour(tempcolumn, ROWS);
    if (check == 1) { //change
      PlayerOneWon = true;
    }
    if (check == 2) { //change
      PlayerTwoWon = true;
    }
  }
  delayAndLight(2);

  //checking horizontal
  Serial.println("Check Horizontal Win");
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
    //    for (int j = 0; j < (COLUMNS * 3) - 3; j++) {
    //      int checkfour[4] = {};
    //      for (int k = 0; k < 4; k ++) {
    //        checkfour[k] = tempHorizontalRow[j + k];
    //      }
    int result = checkifArrayContainsFour(tempHorizontalRow, COLUMNS * 3);
    if (result == 1) {
      PlayerOneWon = true;
    }
    if (result == 2) {
      PlayerTwoWon = true;
    }

    //}
  }
  delayAndLight(2);

  Serial.println("Check Diagonal Win");

  //check diagonal
  for (int i = 0; i < (COLUMNS * 3) - 3; i++) {
    for (int j = 0; j < ROWS - 3; j++) {
      int checkfour[4] = {};
      for (int k = 0; k < 4; k ++) {
        checkfour[k] = horizontalDiagonalCheck[j + k][i + k];
      }
      int result = checkifArrayContainsFour(checkfour, 4);
      if (result == 1) {
        PlayerOneWon = true;
      }
      if (result == 2) {
        PlayerTwoWon = true;
      }
    }
  }
  delayAndLight(2);


  for (int i = 0; i < (COLUMNS * 3) - 3; i++) {
    for (int j = 5; j > 2; j--) {
      int checkfour[4] = {};
      for (int k = 0; k < 4; k ++) {
        checkfour[k] = horizontalDiagonalCheck[j - k][i + k];
      }
      int result = checkifArrayContainsFour(checkfour, 4);
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
  else if (PlayerOneWon == true) {
    gameState = P1_won;
  }
  else if (PlayerTwoWon == true) {
    gameState = P2_won;
  }

  if (isBoardFull()) {
    gameState = tie;
  }
  Serial.println("Finish isGameWon() - Game State: " + String(gameState));
  delayAndLight(5);
}
