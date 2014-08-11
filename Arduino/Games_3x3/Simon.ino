// **************************
// *         Simon          *
// **************************

void playSimon() {  

  byte MAXLEVEL = 100;              // Maximale Durchgänge
  int simonSequence[MAXLEVEL];      // Generierte Sequenz, die es zu lösen gilt

  //Sprachausgabe "Simon"
  trigger.trigger(11);
  delay(2000);

  do {                      // Schleife solange man weitermachen will
  
    fertig = 0;             // zurücksetzen des Fertig-Status
    level = 1;              // Startlevel
    
    byte diffsimon = selectLevel();      // Auswahl der Schwierigkeitsstufe (4 oder 8 Tasten)

    randomSeed(millis());   // genrieren der Sequenz
    for (int i = 0; i < MAXLEVEL; i++) {
      simonSequence[i] = mapSimon(random(0,diffsimon));  // Zufahllszahl und Aufruf der Map-Funktion
    }
    
    do {    // solange weitermachen, bis Fehler
    
      showSimonSequence(simonSequence);  // anzeigen der Sequenz
      getSimonSequence(simonSequence);   // abfragen der Benutzereingaben
  
      delay(1000);          // Pause nach fertiger Eingabe einer Sequenz
      level++;              // nächster Level erreicht
   
    } while(fertig == 0);

    // Ausgabe der erreichten Punktezahl
    trigger.trigger(4);        // Sprachausgabe: "erreichte Punkte"
    playVictory();
    trigger.trigger(level+98);   // Sprachausgabe: Punkestand (Level)
    delay(1200);

  } while (playAgain() == true);  // Nocheinmal?
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

