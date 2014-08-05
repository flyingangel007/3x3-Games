/*
Title: 3x3 - Games
Autor: Flying Angel
Datum: 31.5.2014

Games:
 - Lights Out
 - Simon
 - TicTacToe (noch nicht realisiert!)
 
Updates:

0.3:
 - Sprachausgabe
 - Menu





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
SoftwareSerial trigSerial = SoftwareSerial(6, 5);


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


// Definitionen für LightsOut
#define SPEED_FW 40          // Geschwindigkeit für Farbwechsel bei LightsOut

// Definitionen für Simon
#define MAXLEVEL 100
int sequence[MAXLEVEL];      // Generierte Sequenz die es zu lösen gilt
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
  showField();

  trigger.trigger(1);
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
  byte key = 0;              // warten auf Tastendruck
  while (key == 0) {
    key = kpd.getKey();
  }

  switch (key) {             // je nach Tastendruck
    case 1:                  // Simon
      dimmoutField();
      playSimon();
      break;
    case 3:                  // Lights Out
      dimmoutField();
      playLightsOut();
      break;
    case 7:                  // TicTacToe
      break;    
  }
}

// **************************
// *       Lights Out       *
// **************************

void playLightsOut() {

  //Sprachausgabe "Lights Out"
  trigger.trigger(12);

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

  randomSeed(analogRead(5));

  for (int i=0; i < NUM_LEDS; i++) {
    field[i] = random(2) - 1;
  }
}



// **************************
// *         Simon          *
// **************************

void playSimon() {  
  
  //Sprachausgabe "Lights Out"
  trigger.trigger(11);

  fertig = 0;             // zurücksetzen des Fertig-Status
  level = 1;              // Startlevel
  
  selectDiffSimon();      // Auswahl der Schwierigkeitsstufe (4 oder 8 Tasten)
  
  while(fertig == 0) {    // solange weitermachen, bis Fehler
  
    if (level == 1) generateSimonSequence();  // generieren der Sequenz

    showSimonSequence();  // anzeigen der Sequenz
    getSimonSequence();   // abfragen der Benutzereingaben

    delay(1000);          // Pause nach fertiger Eingabe einer Sequenz
    level++;              // nächster Level erreicht
 
  }
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

  dimmoutField();
  delay(1000);            // Pause vor dem Start
}


// erstellen der Spiel-Sequenz
void generateSimonSequence() {

  randomSeed(analogRead(4));
   
  for (int i = 0; i < MAXLEVEL; i++) {
    sequence[i] = mapSimon(random(0,diffsimon));  // Zufahllszahl und Aufruf der Map-Funktion
  }
}


// Anzeigen der zu merkenden Sequenz
void showSimonSequence() {
  
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
void getSimonSequence() {
  
  int flag = 0;  // alles richtig?
  for (int i = 0; i < level; i++) {

//Serial.println(sequence[i]);
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
void dimmoutField() {

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

