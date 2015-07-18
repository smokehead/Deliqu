/*
 * Sketch um einen Rotary-Encoder mit Push-Taste auszuwerte
 * und den aktuellen Wert auf einem 4-Stelligen Display
 * anzuzeigen.
 * Der Encoder wird über die beiden Interrupt-Pins ausge-
 * wertet. Der Push-Button wird normal ausgewertet.
 * Zunächst werden 4 Nullen angezeigt. Wenn der Encoder in
 * einen negativen Wert wechseln würde wird der Wert 0
 * gesetzt.
 * Der PushButton schaltet auf einen 2. Wert um, der
 * eingestellt werden kann.
 * 
*/
// Zunächst die Pins alle Definieren
enum PinAssignments {
rechtsPin   = 2,   //erkennt Rechtsdrehung
linksPin    = 3,   //erkennt Linksdrehung
pushBut     = 5,   //Druck auf den Button
taktPin     = 8,   //Anschluss SH_CP
speicherPin = 9,   //Anschluss ST_CP
datenPin    = 10   //Anschluss DS
};
volatile int encoderPos = 0;           //aktuelle Drehposition
int encoderPos_A = 0;                  //Position des Drehers 1
int encoderPos_B = 0;                  //Position des Drehers 2
int lastReportedPos = 1;               //Änderungserkennung
static boolean rotating=false;         //Entprellung (Teil)
boolean A_set = false;                 //Richtung A Interrupt
boolean B_set = false;                 //Richtung B Interrupt
boolean C_set = false;
unsigned long tstamp;                  //Zeitstempel
int intervall = 500;                   //Latenzzeit f. Taster und
byte pos_kz    = '0';                   //Welche Einstellung?

//------------------------------------------------------
void setup() {
  pinMode(rechtsPin, INPUT); 
  pinMode(linksPin, INPUT); 
  pinMode(pushBut, INPUT);
  pinMode(taktPin, OUTPUT);
  pinMode(speicherPin, OUTPUT);
  pinMode(datenPin, OUTPUT);
// Interne Pullups einschalten
  digitalWrite(rechtsPin, HIGH);
  digitalWrite(linksPin, HIGH);
  digitalWrite(pushBut, HIGH);
// Registrieren der Interrupt-Routinen
  attachInterrupt(0, doEncoderA, CHANGE); //Pin 2
  attachInterrupt(1, doEncoderB, CHANGE); //Pin 3
//
  tstamp = millis();
}

void loop() {
  rotating = true;
// Aktuell gültigen Wert anzeigen
    zahl_anzeigen(encoderPos);
// Hat sich etwas getan??
// Dann den Wert in der entsprechenden Variablen ablegen
  if (lastReportedPos != encoderPos) {
    lastReportedPos = encoderPos;
    if (pos_kz == '0'){
      encoderPos_A = encoderPos;
    } else {
      encoderPos_B = encoderPos;
    }
  }
// Nun wird sich um den Taster gekümmert.
// Nur wenn seit dem letzten Druck mindestens 500ms
// vergangen sind, wird überhaupt geschaut.
  if ((millis() - tstamp) > intervall) {
    if (digitalRead(pushBut) == HIGH) {
      tstamp = millis();
      if (!C_set){
        if (pos_kz == '0'){
          pos_kz = '1';
          encoderPos = encoderPos_B;
        } else {
          pos_kz = '0';
          encoderPos = encoderPos_A;
        }
        C_set = true;
        delay(5);
      }
    } else {
      C_set = false;
    }
  }

}

//-----------------------------------------------------------
//Interruptverarbeitungen
// Interrupt wenn A geändert wird
void doEncoderA(){
  // Entprellung
  if ( rotating ) delay (1);  // Kurze Verzögerung

  // Hat es sich wirklich geändert?
  if( digitalRead(rechtsPin) != A_set ) {  // Nochmal entprellen
    A_set = !A_set;

    // Erhöhen wenn A vor B
    if ( A_set && !B_set ) 
      encoderPos += 1;

    rotating = false;  // Nochmal entprellen
  }
}

// Interrupt bei Änderung von B ansonsten wie A
void doEncoderB(){
  if ( rotating ) delay (1);
  if( digitalRead(linksPin) != B_set ) {
    B_set = !B_set;
    //  Kleiner werden wenn B vor A
    // Bei 0 ist aber Schluss
    if( B_set && !A_set ) 
      encoderPos -= 1;
      if (encoderPos < 0) {
        encoderPos = 0;
      }
    rotating = false;
  }
}

void zahl_anzeigen (int ausgabe) {
// Variablen für die Anzeige
int einer       = 0;   //Werte der entsprechenden Stelle
int zehner      = 0;
int hunderter   = 0;
int tausender   = 0;
// Zerlegen der Zahl in die einer, zehner, hunderter und tausender
tausender = ausgabe / 1000;
if (tausender > 0){
  ausgabe = ausgabe - (tausender*1000);
}
hunderter = ausgabe / 100;
if (hunderter > 0){
  ausgabe = ausgabe - (hunderter*100);
}
zehner = ausgabe / 10;
if (zehner > 0){
  ausgabe = ausgabe - (zehner*10);
}
einer = ausgabe;
//
ziffer_erzeugen(einer, 4);
ziffer_erzeugen(zehner, 3);
ziffer_erzeugen(hunderter, 2);
ziffer_erzeugen(tausender, 1);
}

/* Setzen der verschiedenen Ziffern, kurz die Stelle
einschalten und gleich wieder aus für die nächste Stelle
Das entspricht dem Multiplexen mit Transistoren bei einzelnen
7-Segment Anzeigen. Bei 10mS delay sieht man das Flackern 
schon recht deutlich.
*/
void ziffer_erzeugen(int ziffer, int stelle){
// Bitstellungen der 7-Segment-Anzeigen
// Zunächst die Bitfolgen der Kathoden (2-Register)
byte katArray[] = {B11111111,
                   B10111111, 
                   B11011111,
                   B11101111,
                   B11110111
                  };
// Dann - Dezimal, die werte der Ziffern
byte wertArray[] = {252,   // 0 11111100
                    96,    // 1 01100000
                    218,   // 2
                    242,   // 3
                    102,   // 4
                    182,   // 5
                    190,   // 6
                    224,   // 7
                    254,   // 8
                    246};  // 9
byte wert;
byte katWert;

  wert = wertArray[ziffer];
  katWert = katArray[stelle];
  digitalWrite(speicherPin, LOW);
  shiftOut(datenPin, taktPin, LSBFIRST, katWert);
  shiftOut(datenPin, taktPin, LSBFIRST, wert);
  digitalWrite(speicherPin, HIGH);
  delay(5);
  digitalWrite(speicherPin, LOW);
  katWert = katArray[0];
  shiftOut(datenPin, taktPin, LSBFIRST, katWert);
  shiftOut(datenPin, taktPin, LSBFIRST, wert);
  digitalWrite(speicherPin, HIGH);
}


