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

