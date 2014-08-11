/*
Title: 3x3-Games
Autor: Flying Angel
Datum: 31.5.2014

Games:
 - Lights Out
 - Simon
 - TicTacToe

MP3-Files:
001: select game
002: play again?
003: level
004: points

011: Simon
012: Lights Out
013: Tic Tac Toe

100-200: "0" - "100" 
 
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

0.6.1 (10.8.2014):
- some speech-output added

0.7 (11.8.2014):
- splitted sketch in more files
- modified speech-output
- "Reaction-Test" added


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



void setup() {

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); // LED-Ansteuerung definieren

  // Serial.begin(9600);  // serielle Schnittstelle für Konsole

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
  for(int i=0; i < 4; i++) {
    trigger.trigger(i+11); // Sprachausgabe des entsprechenden Spieles
    clearField();
    colorSet(mapSimon(i));
    showField();
    delay(2000);
  }
  
  // Anzeigen aller möglichen Spiele
  clearField();
  for(int i=0; i < 4; i++) {
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
    case 9:                  // TicTacToe
      dimmoutDisplay();
      playReactionTest();
      break;    
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
  
  for (j=0; j < 256*5 ; j++) {      // insgesamt 5 Durchläufe
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
  
  trigger.trigger(2);   // Sprachausgabe: "nocheinmal spielen?"  

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

// Auswahl der Schwierigkeitsstufe (4 oder 8 Tasten)
byte selectLevel() {
  
  byte difflevel = 4;  
  trigger.trigger(3);   // Sprachausgabe: "Wähle die Schwierigkeit"

  do {
    clearField();
    field[4] = -2;        // mittleres Feld weiss - Bestätigung der Auswahl
    for (int i = 0; i < difflevel; i++) {
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
      if (difflevel == 4) {
        difflevel = 8;
      }
      else {
        difflevel = 4;
      }
    }
  } while(key != 4);      // Taste 4 (Mitte) bestätigt Eingabe

  dimmoutDisplay();
  delay(1000);            // Pause vor dem Start
  return difflevel;
}

