//#include<iostream>
//
//using namespace std;
//
//const int ROWS = 6;
//const int COLUMNS = 8;
//int arr[ROWS][COLUMNS] = {};
//enum Move {reverse, rotate_left, rotate_right, add};
//enum GameState {playing, P1_won, P2_won, tie};
//int temprow[COLUMNS];
//int tempcolumn[ROWS];
//int player = 1; //this value must be updated at every time and can only be 0, 1, 2,: P1= 1, P2 = 2
//GameState gameState = playing;
//int horizontalDiagonalCheck[ROWS][COLUMNS * 3] = {};
//int tempHorizontalRow[COLUMNS * 3];
//
////update this!
//void ReverseLights(int column);
//void RotateLeft(int row);
//void makeMove(Move m, int column, int row);
//void winSequence();
//void resetGame();
//void switchPlayer();
//void copyRow(int row);
//void copyColumn(int column);
//void ReverseLights(int column);
//void RotateLeft(int row);
//void RotateRight(int row);
//void AddPiece(int column);
//int checkifArrayContainsFour(int temparr[]);
//void copyBigHorizontal(int row);
//void isGameWon();
//bool areBoardsEqual(int arrone[ROWS][COLUMNS], int arrtwo[ROWS][COLUMNS]);
//
////Terribly Written "Unit-Testing" Program
////don't flame Anthony he's a Java person not a C++ person
////also standard libraries don't work on Arduino or something so he had to code all the stuff
//
//
////test helper functions
//bool areBoardsEqual(int arrone[ROWS][COLUMNS], int arrtwo[ROWS][COLUMNS]) {
//  for (int i = 0; i < COLUMNS; i += 1) {
//    for (int j = 0; j < ROWS; j += 1) {
//      if(arrone[j][i] != arrtwo[j][i]){
//        return false;
//      }
//    }
//  }
//  return true;
//}
//
//void copyArrayState(int fromArr[ROWS][COLUMNS]){
//  for (int i = 0; i < COLUMNS; i += 1) {
//    for (int j = 0; j < ROWS; j += 1) {
//      arr[j][i] = fromArr[j][i];
//    }
//  }
//}
//
//bool checkAddPiece(){
//
//}
//
////begin test methods
//bool checkClearMethod(){
//  for (int i = 0; i < COLUMNS; i += 1) {
//    for (int j = 0; j < ROWS; j += 1) {
//      if(arr[j][i] !=0){
//        return false;
//      }
//    }
//  }
//  return true;
//}
//
//bool checkRotateLights(){
//  bool passed = false;
//  int rotateLeft[6][8] = { {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {2 , 1 , 1, 2 , 1 , 2 , 1, 2 } };
//
//  int rotateLeftOut[6][8] = { {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {1 , 1 , 2, 1 , 2 , 1 , 2, 2 } };
//
//  copyArrayState(rotateLeft);
//  makeMove(rotate_left, 0, 5);
//  isGameWon();
//
//  if(gameState == playing && areBoardsEqual(arr, rotateLeftOut)){
//    passed = true;
//  }
//  else {
//    cout << "Rotate Left Failed " << "\n";
//  }
//
//  int rotateRight[6][8] = { {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {1 , 2 , 0, 0 , 0 , 0 , 0, 0},
//  {2 , 1 , 0, 2 , 1 , 2 , 0, 0},
//  {1 , 2 , 0, 1 , 1 , 2 , 2, 0 } };
//
//  int rotateRightOut[6][8] = { {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {1 , 2 , 0, 0 , 0 , 0 , 0, 0},
//  {2 , 1 , 0, 2 , 1 , 2 , 0, 0},
//  {1 , 2 , 0, 1 , 1 , 2 , 2, 0 } };
//
//  copyArrayState(rotateRight);
//  makeMove(rotate_right, 0, 4);
//  isGameWon();
//  if(gameState == playing && areBoardsEqual(arr, rotateRightOut)){
//    passed = true;
//  }
//  else {
//    cout << "Rotate Right Failed " << "\n";
//  }
//  return passed;
//
//
//}
//
//bool checkReverseLights(){
//  bool passed = false;
//  int flipTest [6][8] = { {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 2, 0},
//  {0 , 2 , 1, 0 , 0 , 0 , 1, 2},
//  {2 , 2 , 2, 1 , 1 , 1 , 2, 1},
//  {2 , 1 , 1, 1 , 2 , 2 , 1, 2} };
//  copyArrayState(flipTest);
//  makeMove(reverse, 1, 0);
//  int flipTestOut[6][8] = { {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 2, 0},
//  {0 , 1 , 1, 0 , 0 , 0 , 1, 2},
//  {2 , 2 , 2, 1 , 1 , 1 , 2, 1},
//  {2 , 2 , 1, 1 , 2 , 2 , 1, 2} };
//
//  isGameWon();
//  if(gameState == P2_won && areBoardsEqual(arr, flipTestOut)){
//    passed = true;
//  }
//  return passed;
//
//}
//
//bool checkGameStates(){
//  int tieTest[6][8] = { {1 , 0 , 0, 0 , 0 , 0 , 0, 2},
//  {1 , 0 , 0, 0 , 0 , 0 , 0, 2},
//  {1 , 0 , 0, 0 , 0 , 0 , 0, 2},
//  {1 , 0 , 0, 0 , 0 , 0 , 0, 2},
//  {1 , 0 , 0, 0 , 0 , 0 , 0, 2},
//  {1 , 0 , 0, 0 , 0 , 0 , 0, 2 } };
//
//  int vertTest[6][8] = { {1 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {1 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {1 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {1 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {2 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {2 , 2 , 0, 0 , 0 , 0 , 0, 0 } };
//
//  int horizontalTest[6][8] = { {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 1 , 0 , 0 , 0, 0},
//  {0 , 1 , 1, 1 , 2 , 2 , 2, 2 } };
//
//  int diagonalTest[6][8] = { {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 1 , 0, 0},
//  {0 , 0 , 0, 0 , 1 , 1 , 0, 0},
//  {0 , 0 , 0, 1 , 2 , 2 , 0, 0},
//  {0 , 0 , 1, 2 , 1 , 2 , 2, 0 } };
//
//  int noWin[6][8] = { {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {1 , 2 , 0, 0 , 0 , 0 , 0, 0},
//  {2 , 1 , 0, 2 , 1 , 2 , 0, 0},
//  {1 , 2 , 0, 1 , 1 , 2 , 2, 0 } };
//
//  int horizontalSide[6][8] = { {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {0 , 0 , 0, 0 , 0 , 0 , 0, 0},
//  {1 , 1 , 2, 2 , 0 , 2 , 1, 1} };
//
//  bool failed = false;
//  arr = tieTest;
//  isGameWon();
//  if(gameState != tie){
//    cout << "Tie Scenario Failed" << "\n";
//    failed = true;
//  }
//
//  arr = vertTest;
//  isGameWon();
//  if(gameState != P1_won){
//    cout << "Vertical Win Failed" << "\n";
//    failed = true;
//  }
//
//  arr = horizontalTest;
//  isGameWon();
//  if(gameState !=  P2_won){
//    cout << "Horizontal Win Failed" << "\n";
//    failed = true;
//  }
//
//  arr = diagonalTest;
//  isGameWon();
//  if(gameState != P1_won){
//    cout << "Diagonal Win Failed" << "\n";
//    failed = true;
//  }
//
//  arr = noWin;
//  isGameWon();
//  if(gameState != playing){
//    cout << "Playing Condition Failed" << "\n";
//    failed=  true;
//  }
//
//  arr = horizontalSide;
//  isGameWon();
//  if(gameState != P1_won){
//    cout << "Wrap Win Condition Failed" << "\n";
//    failed=  true;
//  }
//
//
//  return failed;
//}
//
////TODO ADD TEST FOR ADDING PIECES
//
//int main()
//{
//  std::cout << std::boolalpha;
//
//  cout << "Check Win/Loss/Tie/Play State Passed?" << "\n";
//  cout << checkGameStates() << "\n";
//
//  cout << "Check Clear Game State Passed?" << "\n";
//  clearGameState();
//  cout << checkClearMethod() << "\n";
//
//  cout << "Check Flip Move Passed?" << "\n";
//  clearGameState();
//  cout << checkReverseLights() << "\n";
//
//  cout << "Check Rotations Passed?" << "\n";
//  clearGameState();
//  cout << checkRotateLights() << "\n";
//
//  return 0;
//}
//
//
////begin actual methods to test
//
//
//
//
//
//
//
//
//}
