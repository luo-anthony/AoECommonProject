const int ROWS = 6;
const int COLUMNS = 8;
int arr[ROWS][COLUMNS] = {};
enum Move {reverse, rotate_left, rotate_right, add};
enum GameState {playing, P1_won, P2_won, tie};
int temprow[COLUMNS];
int tempcolumn[ROWS];
int player = 1; //this value must be updated at every time and can only be 0, 1, 2,: P1= 1, P2 = 2
GameState gameState = playing;
int horizontalDiagonalCheck[ROWS][COLUMNS * 3] = {};
int tempHorizontalRow[COLUMNS * 3];

void setup() {
  // put your setup code here, to run once:
  clearGameState();
  Serial.begin(9600);
}

void clearGameState(){
    for (int i = 0; i < COLUMNS * 3; i += COLUMNS) {
    for (int j = 0; j < ROWS; j += 1) {
      arr[j][i] = 0;
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(millis());
  for(int i=0; i < 100; i ++){
    isGameWon();
  }
  Serial.println(millis());
}

void makeMove(Move m, int column, int row) {
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
      AddPiece(column);
      break;
  }
  isGameWon();
  if (!gameState == playing) {
    //finish this
  }
  switchPlayer();

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

void AddPiece(int column) {
  for (int i = 5; i >= 0; i --) {
    if (arr[i][column] == 0) {
      arr[i][column] = true;
    }
    break;
  }
}

//int count(int temparr[], int val) {
//  int count = 0;
//  int n = sizeof(temparr) / sizeof(temparr[0]);
//  for (int i = 0; i < n; i ++) {
//    if (temparr[i] == val) {
//      count++;
//    }
//  }
//  return count;
//}


/*
   Returns 0 if neither player has 4, returns 1 if player 1 has 4 in a row, returns 2 if player 2 has 4 in a row
*/
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
