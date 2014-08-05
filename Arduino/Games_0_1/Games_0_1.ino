/*
Title: 3x3 - Games
Autor: Flying Angel
Datum: 31.5.2014

Games:
 - Lights Out
 - Simon

*/

#include <FastLED.h>
#include <Keypad.h>

// Allgemeine Definitionen
#define NUM_LEDS 9           // Anzahl der LEDs definieren
#define DATA_PIN 3           // Data-Pin am Arduino
#define BRIGHTNESS 255       // Helligkeit des Stripes (0-255)
const byte ROWS = 3;         // drei Reihen
const byte COLS = 3;         // drei Spalten

CRGB leds[NUM_LEDS];         // Array mit RGB Farbwerte der einzelnen LEDs
int field[NUM_LEDS];         // Array mit Hue-Werten der einzelnen LEDs (-1 = aus)

#define SPEED_V 1            // Geschwindigkeit der Anzeige bei Victory

// Definitionen für LightsOut
#define SPEED_FW 40          // Geschwindigkeit für Farbwechsel bei LightsOut

// Definitionen für Simon
#define MAXLEVEL 100
int sequence[MAXLEVEL];  // Generierte Sequenz die es zu lösen gilt
int fertig = 0;              // solange weitermachen bis fertig = 1
int level = 1;               // Startlevel  


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



void setup() {
  Serial.begin(9600);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); // LED-Ansteuerung definieren

}


void loop() {

  playSimon();
  playLightsOut();

}



// **************************
// *       Lights Out       *
// **************************

void playLightsOut() {

  int counter = 0;  // Anzahl der noch leuchtenden Lampen
    
  createLightsOut();

  // Schleife, solange noch Felder aktiv
  do {
    for(int farbe=0; farbe < 256; farbe++) {  // Farbschleife (Tasten im Spiel)
  
      // Tasten abfragen (1-9)
      byte key = kpd.getKey();
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


void createLightsOut() {  // erstellen des Spielfeldes

  for (int i=0; i < NUM_LEDS; i++) {
    randomSeed(analogRead(5));
    field[i] = random(2) - 1;
  }
}



// **************************
// *         Simon          *
// **************************

void playSimon() {  
  
  fertig = 0;   // zurücksetzen des Fertig-Status
  level = 1;    // Startlevel
  
  while(fertig == 0) {    // solange weitermachen, bis Fehler
  
    if (level == 1) generateSimonSequence();  // generieren der Sequenz

    showSimonSequence();  // anzeigen der Sequenz
    getSimon();           // abfragen der Benutzereingaben
    
  }
}


void generateSimonSequence() {  // erstellen der Spiel-Sequenz

  randomSeed(analogRead(4));
   
  for (int i = 0; i < MAXLEVEL; i++) {
    sequence[i] = random(0,4);
    
    switch (sequence[i]) {  // mappen der Sequenz auf die Felder des Spielbrettes
      case 0:                   // für einfachen Level (4 Felder)
        break;
      case 1:
        sequence[i] = 2;
        break;
      case 2:
        sequence[i] = 6;
        break;
      case 3:
        sequence[i] = 8;
        break;
      case 4:                   // für schwierigen Level (8 Felder)
        sequence[i] = 1;
        break;
      case 5:
        sequence[i] = 3;
        break;
      case 6:
        sequence[i] = 5;
        break;
      case 7:
        sequence[i] = 7;
        break;
    }
  }
}


void showSimonSequence() {
  
  for (int i = 0; i < level; i ++) {  // so viele Durchläufe wie Level
    clearField();
    
    ColorSet(sequence[i]);       // dem aktuellen Schritt der Sequenz die passenden Farbe zuordnen

    delay(200);
    showField();
    delay (1000);
    clearField();
    showField();
  }
}


void getSimon() {
  
  int flag = 0;  // alles richtig?
  byte key = 0; 
  for (int i = 0; i < level; i++) {

    Serial.println(sequence[i]);
    key = 0;
    while (key == 0) {       // warten auf Tastendruck
      key = kpd.getKey();
    }
    key --;   // mappen von 1-9 auf 0-8

    // Anzeige der gedrückten Taste
    ColorSet(key);  // der gedrückten Taste die entsprechende Farbe zuweisen
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
  delay(1000);     // Pause nach fertiger Eingabe einer Sequenz
  level++;         // nächster Level erreicht
}



// **************************
// *       Allgemein        *
// **************************

void clearField() {
  
  // alle Felder ausschalten
  for (int i=0; i < NUM_LEDS; i++) {
    field[i] = -1;
  }
}


void showField() {   // Ausgabe aller Felder auf LED-Display

  // Den einzelnen LEDs Feld-Farbe zuweisen
  for (int i=0; i < NUM_LEDS; i++) {
    if (field[i] != -1) {
      leds[i] = CHSV(field[i], 255, BRIGHTNESS); // Hue "Farbe" wenn aktiv
    }
    else {
      leds[i] = CRGB::Black;                     // schwarz wenn inaktiv
    }
  }
  FastLED.show();                                // aktivieren der LEDs
}


void playVictory() {  //Cororcircle
  
  int i, j;
  
  for (j=0; j < 256*8 ; j++) {     
    for (i=0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(((i * 256 / NUM_LEDS) + j) % 256, 255, BRIGHTNESS);
    }  
    FastLED.show();  
    delay(SPEED_V);
  }  
}


void ColorSet(int Feld) {  // einem bestimmten Feld eine fixe Farbe zuordnen  

  switch (Feld) {          // welches Felde?
      case 0:
        field[Feld] = 0;   // rot (Feld 0)
        break;
      case 1:
        field[Feld] = 192; // violett (Feld 1)
        break;
      case 2:
        field[Feld] = 160; // blau (Feld 2)
        break;
      case 3:
        field[Feld] = 224; // pink (Feld 3)
        break;
      case 5:
        field[Feld] = 32;  // orange (Feld 5)
        break;
      case 6:
        field[Feld] = 64;  // gelb (Feld 6)
        break;
      case 7:
        field[Feld] = 128; // aqua (Feld 7)
        break;
      case 8:
        field[Feld] = 96;  // grün (Feld 8)
        break;
    }
}

