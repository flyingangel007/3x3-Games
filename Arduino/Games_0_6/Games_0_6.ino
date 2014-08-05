/*
Title: 3x3-Games 0.6
Autor: Flying Angel
Datum: 31.5.2014

Games:
 - Lights Out
 - Simon
 - TicTacToe

MP3-Files:
001 - select game
002 - play again?
011 - Simon
012 - Lights Out
013 - Tic Tac Toe
 
Updates:

0.3 (11.6.2014):
- mp3-Trigger added
- Menu added
- minor Changes

0.4 (12.6.2014):
- TicTacToe added
- minor Changes

0.5 (13.6.2014):
- "Play again?" added

0.5.1 (14.6.2014):
- Simon: Memory-optimization ("simonSequence" and "MAXLEVEL" no globale definition)

0.5.2 (18.6.2014):
- TicTacToe: changing beginner (player <-> computer)
- Hardware: SoftwareSerial on pin 3 & 4
- small changes

0.6 (18.6.2014):
- TicTacToe: show winner
- small changes


*/

#include <FastLED.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <MP3Trigger.h>

// Allgemeine Definitionen
#define DATA_PIN 2           // Data-Pin am Arduino
#define NUM_LEDS 9           // Anzahl der LEDs definieren
#define BRIGHTNESS 255       // Helligkeit des Stripes (0-255)
const byte ROWS = 3;         // drei Reihen
const byte COLS = 3;         // drei Spalten

CRGB leds[NUM_LEDS];         // Array mit RGB Farbwerte der einzelnen LEDs
int field[NUM_LEDS];         // Array mit Hue-Werten der einzelnen LEDs (-1 = aus, -2 = weiss)
byte key = 0;                // gedrückte Taste

#define SPEED_V 1            // Geschwindigkeit der Anzeige bei Victory


// Serielle Schnittstelle definieren
SoftwareSerial trigSerial = SoftwareSerial(3, 4);


// MP3-Trigger definieren
MP3Trigger trigger;


// Keypad difinieren
// Define the Keymap
byte keys[ROWS][COLS] = {
  {1,2,3},
  {4,5,6},
  {7,8,9},
};
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 9, 8, 7 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 12, 11, 10 }; 
// Create the Keypad
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS );


// Definitionen für Simon
int fertig = 0;              // solange weitermachen bis fertig = 1
int level = 1;               // Startlevel  
byte diffsimon = 4;          // Schwierigkeit: (4 leicht, 8 schwer)



void setup() {

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); // LED-Ansteuerung definieren

  // Start serial communication with the trigger (over SoftwareSerial)
  trigger.setup(&trigSerial);
  trigSerial.begin( MP3Trigger::serialRate() );

  playVictory();


}


void loop() {

  gameMenu();

}


// **************************
// *          Menu          *
// **************************

void gameMenu() {

  clearField();
//  showField();

  trigger.trigger(1);   // Sprachausgabe: "Wähle ein Spiel"
  delay(2000);
  
  // Anzeigen der einzelnen Spiele
  for(int i=0; i < 3; i++) {
    trigger.trigger(i+11); // Sprachausgabe des entsprechenden Spieles
    clearField();
    colorSet(mapSimon(i));
    showField();
    delay(2000);
  }
  
  // Anzeigen aller möglichen Spiele
  clearField();
  for(int i=0; i < 3; i++) {
    colorSet(mapSimon(i));
  }
  showField();

  // Tasten abfragen (1-9)
  key = 0;              // warten auf Tastendruck
  while (key == 0) {
    key = kpd.getKey();
  }

  switch (key) {             // je nach Tastendruck
    case 1:                  // Simon
      dimmoutDisplay();
      playSimon();
      break;
    case 3:                  // Lights Out
      dimmoutDisplay();
      playLightsOut();
      break;
    case 7:                  // TicTacToe
      dimmoutDisplay();
      playTicTacToe();
      break;    
  }
}

// **************************
// *       Lights Out       *
// **************************

void playLightsOut() {

  byte SPEED_FW = 40;  // Geschwindigkeit für Farbwechsel bei LightsOut

  
  //Sprachausgabe "Lights Out"
  trigger.trigger(12);

  do {                 // Schleife solange man weitermachen will

    int counter = 0;   // Anzahl der noch leuchtenden Lampen
      
    createLightsOut();
  
    // Schleife, solange noch Felder aktiv
    do {
      for(int farbe=0; farbe < 256; farbe++) {  // Farbschleife (Tasten im Spiel)
    
        // Tasten abfragen (1-9)
        key = kpd.getKey();
        if(key) {     // Check for a valid key.
          key--;      // mappen von 1-9 auf 0-8
          
          // tausch des Feldes selbst
          if (field[key] != -1) {
            field[key] = -1;           // wenn Feld aktiv -> inaktiv
          }
          else {
            field[key] = farbe;        // wenn Feld inaktiv -> Feld = aktuelle Farbe
          }
            
          // tausch des Feldes darüber
          if (key - ROWS >= 0) {
            if (field[key - ROWS] != -1) {
              field[key - ROWS] = -1;
            }
            else {
              field[key - ROWS] = farbe;
            }
          }
          
          // tausch des Feldes darunter
          if (key + ROWS < NUM_LEDS) {
            if (field[key + ROWS] != -1) {
              field[key + ROWS] = -1;
            }
            else {
              field[key + ROWS] = farbe;
            }
          }
          
          // tausch des Feldes links
          if (key / ROWS == (key - 1) / ROWS) {
            if (field[key - 1] != -1) {
             field[key - 1] = -1;
            }
            else {
              field[key - 1] = farbe;
            }
          }
      
          // tausch des Feldes rechts
          if (key / ROWS == (key + 1) / ROWS) {
            if (field[key + 1] != -1) {
             field[key + 1] = -1;
            }
            else {
              field[key + 1] = farbe;
            }
          }
        }
  
        showField();               // Anzeigen des aktuellen Spielfeldes
  
        // Check ob alles aus ist
        counter = 0;
        for (int i=0; i < NUM_LEDS; i++) {
          if (field[i] != -1) { // ist Feld aktiv?
            counter++;          // hochzählen für jedes aktive Feld
            field[i] = farbe;   // Farbzuweisung für aktives Feld
          }
        }
        if (counter == 0) {break;} // alle Lampen aus -> Farbschleife verlassen
        delay (SPEED_FW);          // Verzögerung für Farbwechsel
      }                            // Ende Farbschleife
    } while(counter != 0);         // weitermachen, solange noch Felder aktiv
    
    // Alle Felder aus -> Anzeige des Siegerschirms
    playVictory();

  }
  while (playAgain() == true);  // Nocheinmal?
}


void createLightsOut() {  // erstellen des Spielfeldes

  randomSeed(millis());

  for (int i=0; i < NUM_LEDS; i++) {
    field[i] = random(2) - 1;
  }
}



// **************************
// *         Simon          *
// **************************

void playSimon() {  

  byte MAXLEVEL = 100;              // Maximale Durchgänge
  int simonSequence[MAXLEVEL];      // Generierte Sequenz, die es zu lösen gilt

  //Sprachausgabe "Simon"
  trigger.trigger(11);

  do {                      // Schleife solange man weitermachen will
  
    fertig = 0;             // zurücksetzen des Fertig-Status
    level = 1;              // Startlevel
    
    selectDiffSimon();      // Auswahl der Schwierigkeitsstufe (4 oder 8 Tasten)

    randomSeed(millis());   // genrieren der Sequenz
    for (int i = 0; i < MAXLEVEL; i++) {
      simonSequence[i] = mapSimon(random(0,diffsimon));  // Zufahllszahl und Aufruf der Map-Funktion
    }
    
    while(fertig == 0) {    // solange weitermachen, bis Fehler
    
      showSimonSequence(simonSequence);  // anzeigen der Sequenz
      getSimonSequence(simonSequence);   // abfragen der Benutzereingaben
  
      delay(1000);          // Pause nach fertiger Eingabe einer Sequenz
      level++;              // nächster Level erreicht
   
    }
  }
  while (playAgain() == true);  // Nocheinmal?
}


void selectDiffSimon() {  // Auswahl der Schwierigkeitsstufe (4 oder 8 Tasten)
  
  do {
    clearField();
    field[4] = -2;        // mittleres Feld weiss - Bestätigung der Auswahl
    for (int i = 0; i < diffsimon; i++) {
      colorSet(mapSimon(i));
    }
  
    showField();          // Anzeigen der zu verwendenden Felder
  
    key = 0;              // warten auf Tastendruck
    while (key == 0) {
      key = kpd.getKey();
    }
    key --;               // mappen von 1-9 auf 0-8

    // Tasten 1, 3, 5, 7 schalten zwischen leicht und schwer um
    if (key == 1 || key == 3 || key == 5 || key == 7) {
      if (diffsimon == 4) {
        diffsimon = 8;
      }
      else {
        diffsimon = 4;
      }
    }
  } while(key != 4);      // Taste 4 (Mitte) bestätigt Eingabe

  dimmoutDisplay();
  delay(1000);            // Pause vor dem Start
}


// Anzeigen der zu merkenden Sequenz
void showSimonSequence(int sequence[]) {
  
  for (int i = 0; i < level; i ++) {  // so viele Durchläufe wie Level
    clearField();
    
    colorSet(sequence[i]);       // dem aktuellen Schritt der Sequenz die passenden Farbe zuordnen

    delay(200);
    showField();
    delay (1000);
    clearField();
    showField();
  }
}


// abfragen der Spieler-Sequenz
void getSimonSequence(int sequence[]) {
  
  for (int i = 0; i < level; i++) {

    key = 0;
    while (key == 0) {       // warten auf Tastendruck
      key = kpd.getKey();
    }
    key --;   // mappen von 1-9 auf 0-8

    // Anzeige der gedrückten Taste
    colorSet(key);  // der gedrückten Taste die entsprechende Farbe zuweisen
    showField();   
    delay(200);          // solange wird gedrückte Taste bestätigt 
    clearField();
    showField();

    if (key != sequence[i]) {
      fertig = 1;    
      playVictory();
      return;
    }
  }
}


int mapSimon(int posTaste) {  // mappen von 0-8 auf die Felder des Spielbrettes

  switch (posTaste) {
    // für einfachen Level (4 Felder)
    case 0:            // Ecke links oben
      break;
    case 1:            // Ecke rechts oben
      posTaste = 2;
      break;
    case 2:            // Ecke links unten
      posTaste = 6;
      break;
    case 3:            // Ecke rechts unten
      posTaste = 8;
      break;
    // für schwierigen Level (8 Felder)
    case 4:            // Mitte oben
      posTaste = 1;
      break;
    case 5:            // Mitte links
      posTaste = 3;
      break;
    case 6:            // Mitte rechts
      posTaste = 5;
      break;
    case 7:            // Mitte unten
      posTaste = 7;
      break;
    case 8:            // Mitte
      posTaste = 4;
      break;
  }
  return posTaste;     // neue Position wird zurückgegeben  
}



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


// **************************
// *       Allgemein        *
// **************************

// alle Felder ausschalten
void clearField() {
  
  for (int i=0; i < NUM_LEDS; i++) {
    field[i] = -1;
  }
}


// Ausgabe aller Felder auf LED-Display
void showField() {   
  
  // Den einzelnen LEDs Feld-Farbe zuweisen
  for (int i=0; i < NUM_LEDS; i++) {
    if (field[i] >= 0) {
      leds[i] = CHSV(field[i], 255, BRIGHTNESS); // Hue "Farbe" wenn aktiv
    }
    else {
      if (field[i] == -1) {
        leds[i] = CRGB::Black;                   // schwarz wenn -1
      }  
      else {
        leds[i] = CRGB::White;                   // weiß wenn -2
      }
    }
  }
  FastLED.show();                                // aktivieren der LEDs
}


// ausdimmen des gesamten Feldes
void dimmoutDisplay() {

  for (int hell=255; hell >= 0; hell--) {
    
    // Den einzelnen LEDs Feld-Farbe zuweisen
    for (int i=0; i < NUM_LEDS; i++) {
      if (field[i] >= 0) {
        leds[i] = CHSV(field[i], 255, hell); // Hue "Farbe" wenn aktiv
      }
      else {
        if (field[i] == -1) {
          leds[i] = CRGB::Black;                   // schwarz wenn -1
        }  
        else {
          leds[i] = CHSV(0, 0, hell);              // weiß wenn -2
        }
      }
    }
    FastLED.show();                                // aktivieren der LEDs
    delay(5);
  }  
}


// Cororcircle
void playVictory() {  
  
  int i, j;
  
  for (j=0; j < 256*8 ; j++) {      // insgesamt 8 Durchläufe
    for (i=0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(((i * 256 / NUM_LEDS) + j) % 256, 255, BRIGHTNESS);
    }  
    FastLED.show();  
    delay(SPEED_V);
  }
  clearField();
  showField();  
}


// einem bestimmten Feld eine fixe Farbe zuordnen
void colorSet(int Feld) {  

  switch (Feld) {          // welches Felde?
      case 0:
        field[Feld] = 0;   // rot (Feld 0)
        break;
      case 1:
        field[Feld] = 224; // pink (Feld 1)
        break;
      case 2:
        field[Feld] = 160; // blau (Feld 2)
        break;
      case 3:
        field[Feld] = 128; // aqua (Feld 3)
        break;
      case 5:
        field[Feld] = 32;  // orange (Feld 5)
        break;
      case 6:
        field[Feld] = 64;  // gelb (Feld 6)
        break;
      case 7:
        field[Feld] = 192; // violett (Feld 7)
        break;
      case 8:
        field[Feld] = 96;  // grün (Feld 8)
        break;
    }
}


// nocheinmal spielen?
boolean playAgain() {
  
  clearField();
  field[0] = 0;
  field[8] = 96;
  showField();
  
  // Tasten abfragen (1-9)
  byte key = 0;                     // warten auf Tastendruck
  while (key != 1  && key != 9) {   // bis Taste 1 oder 9 gedrückt wird
    key = kpd.getKey();
  }
  
  if(key == 1) {
    dimmoutDisplay();
    return false;
  }
  dimmoutDisplay();  
  return true;
}

