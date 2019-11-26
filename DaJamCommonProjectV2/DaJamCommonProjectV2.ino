  const int ROWS = 6;
  const int COLUMNS = 8;
  int arr[ROWS][COLUMNS] = {};
  enum Move {reverse, rotate_left, rotate_right, add, nomove};
  enum GameState {playing, P1_won, P2_won, tie};
  enum JoystickMove = {up, down, left, right, center};
  int temprow[COLUMNS];
  int tempcolumn[ROWS];
  int player = 1; //this value must be updated at every time and can only be 0, 1, 2,: P1= 1, P2 = 2
  GameState gameState = playing;
  int horizontalDiagonalCheck[ROWS][COLUMNS * 3] = {};
  int tempHorizontalRow[COLUMNS * 3];

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
  bool confirmButton = false; //TO BE IMPLEMENTED - confirms joystick move

  void ReverseLights(int column);
  void RotateLeft(int row);
  bool makeMove(Move m, int column, int row);
  void winSequence();
  void resetGame();
  void switchPlayer();
  void copyRow(int row);
  void copyColumn(int column);
  void ReverseLights(int column);
  void RotateLeft(int row);
  void RotateRight(int row);
  bool AddPiece(int column);
  int checkifArrayContainsFour(int temparr[]);
  void copyBigHorizontal(int row);
  void isGameWon();


  void setup() {
    // put your setup code here, to run once:
    clearGameState();
    Serial.begin(9600);
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

  //start backend code

  void clearGameState(){
      for (int i = 0; i < COLUMNS; i += 1) {
      for (int j = 0; j < ROWS; j += 1) {
        arr[j][i] = 0;
      }
    }
  }

  Move nextMove = nomove;
  JoystickMove lastDirection = center;
  int move_column = -1;
  int move_row = -1;
  void loop() {
    while(!isGameWon()){
      bool validMove = false;

      while(!validMove){
        bool inputReceived = false;
        while(nextMove == nomove){
          parseInputs();
        }
        //prompt user for Move
        //pressing button drops PIECE
        //animations need to call lights_drawBoard
        if(!validMove){
          //flash lights in certain "noob sequence"
        } else {
          validMove = makeMove(nextMove, int FILLER, int FILLER);
        }
      }
      resetInputs();
      lights_drawBoard();
      switchPlayer();
    }
    winSequence();
    resetGame();
  }

  bool joystickTriggered = false;
  JoystickMove last_partialDirection = center;
  int boardPointerX = 0;
  int boardPointerY = 0;
  void resetInputs(){
    move_row = -1;
    move_column = -1;
    joystickTriggered = false;
    last_partialDirection = center;
    nextMove = nomove;
    lastDirection = center;
    boardPointerX = 0;
    boardPointerY = 0;
    confirmButton = false;
  }


  bool parseInputs(){
    int column = -1;
    readButtons();
    for(int i=0; i < 8; i ++){
      if(buttonReads[i] == 1){
        column = i;
        nextMove = add;
        break;
      }
    }
    if(column==-1 && !confirmButton ){
      readJoystick();
      if(!joystickTriggered){
        if(joystickX >40){
          joystickTriggered = true;
          last_partialDirection = right;
        }
        else if(joystickX < -40){
          joystickTriggered = true;
          last_partialDirection = left;
        }
        else if(joystickY > 40){
          joystickTriggered = true;
          last_partialDirection = up;
        }
        else if(joystickY < -40){
          joystickTriggered = true;
          last_partialDirection = down;
        }
      }
      else {
        if(joystickY < 15 && joystickY > -15 && joystickX < 15 && joystickX > -15){
          lastDirection = last_partialDirection;
          switch(lastDirection){
            case up:
              boardPointerY = boardPointerY + 1 % 6;
            case down:
              boardPointerY--;
              if(boardPointerY <0){
                boardPointerY = 5;
              }
            case right:
              boardPointerX = boardPointerX + 1 % 8;
            case left:
              boardPointerX--;
              if(boardPointerX <0){
                boardPointerX = 7;
              }
          }
        }
      }

    }
    else if(confirmButton && column ==-1){
      if(lastDirection == left || lastDirection == right){
        
      }
    }


    lights_drawBoard();
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

  }

  void resetGame() {
    clearGameState();
    int player = 1;
    gameState = playing;

  }

  void switchPlayer() {
    if (player == 1) {
      player = 2;
    }
    else {
      player = 1;
    }
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

  void ReverseLights(int column) {
    copyColumn(column);
    int n = sizeof(tempcolumn) / sizeof(tempcolumn[0]);
    int numzeros = (tempcolumn, tempcolumn + n, 2);
    int onetwos[ROWS - numzeros];
    for (int i = numzeros; i < ROWS; i++) {
      onetwos[i - numzeros] = tempcolumn[i];
    }
    for (int i = 0; i < ROWS; i ++) {
      arr[i][column] = 0;
    }
    for (int i = 0; i < ROWS - numzeros; i ++) {
      arr[ROWS - 1 - i][column] = onetwos[i];
    }
    //NEED TO CHECK IF THIS WORKS LOL
  }


  void RotateLeft(int row) {
    copyRow(row);
    for (int i = 0; i < 7; i ++) {
      arr[row][i] = temprow[i + 1];
    }
    arr[row][COLUMNS - 1] = temprow[0];
  }

  void RotateRight(int row) {
    copyRow(row);
    for (int i = 1; i < COLUMNS; i ++) {
      arr[row][i] = temprow[i - 1];
    }
    arr[row][0] = temprow[COLUMNS - 1];
  }

  /*
   Returns true if valid move, false if the column is full already
  */
  bool AddPiece(int column) {
    //TODO WHAT IF THEY ADD A PIECE TO A FULL COLUMN?!
    for (int i = 5; i >= 0; i --) {
      if (arr[i][column] == 0) {
        if(i==0){
          return false;
        }
        arr[i][column] = true;
      }
      break;
    }
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

    //checking horizontal

    //copy arr into triple state
    for (int i = 0; i < COLUMNS * 3; i += COLUMNS) {
      for (int j = 0; j < ROWS; j += 1) {
        for (int k = 0; k < COLUMNS; k += 1) {
          horizontalDiagonalCheck[j][i + k] = arr[j][k];
        }
      }
    }

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

    for (int i = 3; i < (COLUMNS * 3); i++) {
      for (int j = 3; j < ROWS; j++) {
        int checkfour[4] = {};
        for (int k = 0; k < 4; k ++) {
          checkfour[k] = horizontalDiagonalCheck[j -k][i -k];
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

    if (PlayerOneWon == true && PlayerTwoWon == true) {
      gameState = tie;
    }
    if (PlayerOneWon == true) {
      gameState = P1_won;
    }
    if (PlayerTwoWon == true) {
      gameState = P2_won;
    }






  }
