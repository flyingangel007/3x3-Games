// **************************
// *     Reaction Test      *
// **************************

void playReactionTest() {
  byte MAXLEVEL = 0;
  
  //Sprachausgabe "Reaktions Test"
  trigger.trigger(12);

  do {                      // Schleife solange man weitermachen will

    byte level = 0;
    boolean fertig = 0;
    byte diffReact = selectLevel();  // Auswahl der Schwierigkeit

    do {
      
      randomSeed(millis());
      delay(random(1000,4000));   // variable Pause zwischen 1 und 4 Sekunden
      byte reactKey = mapSimon(random(diffReact));  // generieren einer zufälligen Taste
      
      clearField();               // Anzeige der zu drückenden Taste
      colorSet(reactKey);
      showField();
      
      // abfragen der Taste im Zeitfenster
      byte key = 0;
      unsigned long tempTime = millis();
      fertig = 1;
      while (millis() < tempTime + 1000 - level*10) {  // Schleife für max. 1 Sekunde - dem aktuellen Level*10
        
        key = kpd.getKey();    // abfragen der Tasten
        if (key) {             // verlassen der Schleife sobald Taste gedrückt wurde
          break;
        }
      }
      key--;
      clearField();            // löschen der Anzeige
      showField();
  
      if (key == reactKey) {   // wurde die richtige Taste gedrückt?
        fertig = 0;            // wenn ja noch nicht fertig
        level++;
      }
      
    } while (fertig == 0);     // weiter solange kein Fehler gemacht wurde
    
    // Ausgabe der erreichten Punktezahl
    trigger.trigger(4);        // Sprachausgabe: "erreichte Punkte"
    playVictory();
    trigger.trigger(level+100);   // Sprachausgabe: Punkestand (Level)
    delay(1200);
   
  } while (playAgain() == true);  // Nocheinmal?
}
