#include <FastLED.h>
#include <Keypad.h>

#define NUM_LEDS 9     // Anzahl der LEDs definieren 
#define DATA_PIN 2       // Data-Pin am Arduino
#define BRIGHTNESS 255   // Helligkeit des Stripes (0-255)
#define SPEED 50          // Geschwindigkeit der Anzeige

CRGB leds[NUM_LEDS];

// Keypad difinieren
const byte ROWS = 3; // Four rows
const byte COLS = 3; // Three columns
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
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );



void setup()
{
  Serial.begin(9600);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.show();
}

void loop() { 

  
  //Tasten abfragen
  
  byte key = kpd.getKey();
  if(key)  // Check for a valid key.
  {
    key=key-1;
    if(leds[key]) {
      leds[key] = CRGB::Black;
    }
    else {
      leds[key] = CRGB::Blue;
    }
   
    FastLED.show();
//    Serial.println(key);


    }
  }

