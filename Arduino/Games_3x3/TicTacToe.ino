// **************************
// *      Tic Tac Toe       *
// **************************
void playTicTacToe() {  

  //Sprachausgabe "TicTacToe"
  trigger.trigger(13);
  
  int player = 2;  // flag für ersten Spieler (Spieler=1, Computer=2)
  
  do {             // Schleife solange man weitermachen will
  
    clearField();  // Anzeige löschen
    showField();
    
    int board[] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // leeres Spielfeld erzeugen
     
    for(int turn=0; turn < 9 && win(board) == 0; ++turn) {   // maximal 9 Züge, solange noch niemand gewonnen hat
      if((turn + player)%2 == 0) {             // Computer ist am Zug
        
        if(turn != 0) {                        // nicht der erste Zug -> berechnen
          computerMove(board);
        }
        else {                                 // erster Zug -> zufälliges Feld
          randomSeed(millis());
          board[random(0,9)] = 1;
        }
      }
      else {                                   // Spieler ist am Zug
        drawboard(board);
        playerMove(board);
      }
    }
    switch(win(board)) {        // wer hat gewonnen?
      case 0:                   // Unentschieden
        drawboard(board);
        delay(3000);
        break;
      case 1:                   // Computer hat gewonnen
        showWinner(board); 
        delay(3000);
        break;
      case -1:                  // Spieler hat gewonnen
        playVictory();
        showWinner(board);
        delay(3000);
        break;
    }
    player = abs(player - 3);   // wechsel des ersten Spielers

  }
  while (playAgain() == true);  // Nocheinmal?

}

  
int win(int board[]) {
  
  // Definition der Gewinn-Situationen
  int wins[][8] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
  for(int i=0;i<8;++i) {   // abfragen aller 8 Gewinnsituationen
   
    if(board[wins[i][0]] != 0 && board[wins[i][0]] == board[wins[i][1]] && board[wins[i][0]] == board[wins[i][2]])
      return board[wins[i][2]];  // hat ein Spieler gewonnen Rückmeldung, sonst "0"
  }
  return 0;
}

int minimax(int board[],int player) {
  
  int winner=win(board);
  if(winner!=0)
    return winner*player;

  int moveval=-1;
  int score=-2; //Losing moves are preferred to no move
  for(int i=0;i<9;++i)
  {
    if(board[i]==0)
    {
      board[i]=player;  //Try the move
      int thisScore=-minimax(board,player*-1);
      if(thisScore>score)
      {
        score=thisScore;
        moveval=i;
      }//Pick the one that's worst for the opponent
      board[i]=0;//Reset board after try
    }
  }
  if(moveval==-1) return 0;
  return score;
}

void computerMove(int board[]) {
  
  int moveval=-1;
  int score=-2;
  int i;
  for(i=0;i<9;++i)
  {
    if(board[i]==0)
    {
      board[i]=1;
      int tempScore=-minimax(board,-1);
      board[i]=0;
      if(tempScore>score)
      {
        score=tempScore;
        moveval=i;          // Feld wird Computer zugeordnet
      }
    }
  }
  //returns a score based on minimax tree at a given node.
  board[moveval]=1;
}

void playerMove(int board[]) {
  
  int moveval=0,flag=0;

    while (moveval == 0) {  // warten auf Tastendruck
    moveval = kpd.getKey();
  }
  moveval--;                // mappen der Tasten 1-9 auf 0-8
  
  if(board[moveval]==1 || board[moveval]==-1)
  {
    playerMove(board);      // Feld bereits besetzt
  }
  else if(board[moveval]==0)
  {
    board[moveval]=-1;      // Feld wird Spieler zugeordnet
  }
  drawboard(board);
}


void drawboard(int board[]) {
  
  clearField();
  for (int i=0; i < NUM_LEDS; i++) {
    switch(board[i]) {
      case 0:
        field[i] = -1;     // leeres Feld ist schwarz
        break;
      case -1:
        field[i] = 160;    // Spieler ist blau
        break;
      case 1:
        field[i] = 0;      // Computer ist rot
        break;
    }
  }
  showField();  
}


void showWinner(int board[]) {
  clearField();

  // Definition der Gewinn-Situationen
  int wins[][8] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
  for(int i=0;i<8;++i) {   // abfragen aller 8 Gewinnsituationen
   
    if(board[wins[i][0]] != 0 && board[wins[i][0]] == board[wins[i][1]] && board[wins[i][0]] == board[wins[i][2]]) {
      for(int j=0;j<3;j++) {
        switch(board[wins[i][j]]) {
          case -1:
            field[wins[i][j]] = 160;
            break;
          case 1:
            field[wins[i][j]] = 0;
            break;
        }
      }
      showField();
      break;
    }
  }
}

