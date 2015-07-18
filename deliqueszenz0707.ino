/*
                 D E L I Q U E S Z E N Z
Ermittlung des Deliqueszenzpunktes eines Salzes
Es wird die Luftfeuchtigkeit gesucht, bei der ein Salz
so viel Feuchtigkeit aus der Luft aufnimmt, dass es Leit-
fähig wird.
Zunächst werden die aktuellen Werte ermittelt und die 
Startparameter gesetzt. Dann wird die gewünschte erste
Sollumgeung hergestellt und die Leitfähigkeit gemessen.
Die Luftfeuchtigkeit wird durch Zuschalten eines Befeuchters
so lange erhöht bis die Leitfähigkeit eintritt. Dann ist das
Programm beendet.
*/
#include "DHT.h"                          //Bibliothek mit allem was man zum DHT22-Betrieb braucht
#define DHTPIN1 3                                 //Digitaleingang für Temperatur und Feuchte
#define DHTPIN2 2

#define DHTTYPE DHT22                     //Typ des Sensors

// P a r a m e t e r
//Zeitintervall für die Istzustaandsmeldungen
unsigned long IntervProto;
//Zeitintervall für die Erhöhung der Luftfeuchtigkeit
unsigned long IntervHygro;
//Spannungswert auf den der Referenzwert fallen muss
//   um eine Leitfähigkeit des Salzes zu vermuten
float LeitSpann;
//Inkrement in Prozenpunkten um die Luftfeuchtigkeit 
//   erhöht wird.
int IncrHygro;
//Maximaler Differenzwert der beiden Feuchtemsser um eine
//   gleichmäßige Verteilung zu postulieren
float SensorDiff;
//Arduino Pins für die verschiedenen Aufgaben
//
int spannungsPin;
int BefeuchterPin;
DHT dht1(DHTPIN1, DHTTYPE);    //DHT-Objektreferenz Primär
DHT dht2(DHTPIN2, DHTTYPE);    //DHT-Objektreferenz Sekkundär
// V a r i a b l e n 
//Zeitpunkt der letzten Protokollierung
unsigned long ZeitProt;
//Zeitpunkt der letzten Sollwerterhöhung
unsigned long ZeitSoll;
//Aktueller Feuchtewert Primär
float AktHygr01;
//Aktueller Feuchtewert Sekundär
float AktHygr02;
//Aktuelle Temperatur Primär
float AktTemp;
//gemessener Spannungswert
int SensorValue;
//errechnete Spannung
float Spannung;
//Schalterstellung des Befeuchters
boolean befeuchter;
//Feuchtigkeitssollwert
unsigned long HygroSoll;
/*
-------------------------------------------------------
*/
void setup() {
  IntervProto   = 60000;         // 1 Minute
  IntervHygro   = 1200000;       // 20 Minuten
  LeitSpann     = 2.3;
  IncrHygro     = 5;
  HygroSoll     = 70;            // Bei 5 Prozent geht es los
  SensorDiff    = 3;
  spannungsPin  = 0;
  BefeuchterPin = 7;
  
  ZeitProt = millis();
  ZeitSoll = millis();

  Serial.begin(9600);

  AktHygr01   = dht1.readHumidity();
  Serial.println(AktHygr01);
  AktHygr02   = dht2.readHumidity();
  Serial.println(AktHygr02);
  AktTemp     = dht1.readTemperature();
  SensorValue = 0;
  befeuchter  = false;


// Hat das alles geklappt? Wenn nein kann man eigentlich aussteigen
// Ich leg' mich erst mal 10 Minuten auf's Ohr
  if (isnan(AktHygr01) ||
     isnan(AktHygr02)) {
     Serial.println("Die Feuchtigkeit konnte nicht gelesen werden");
     delay(600000);
  }

// Und den ersten Spannungswert ermitteln
// Wenn das nicht klappt, dann ist hier auch Feierabend
  Spannung = spannung_messen();
  if (Spannung == 0) {
     Serial.println("Es kann keine Spannung ermittelt werden");
     delay(600000);
  }
  
// Entweder hat alles geklappt oder die Zeit ist um
// dann kann man mal zunächst das Protokoll eröffnen
  protokoll_schreiben(1);
  
}
/*
-------------------------------------------------------
*/
void loop() {
/* Aus dieser Funktion kommt das Programm erst zurück, wenn
   der Sollwert erreicht und der Befeuchter wieder abge-
   schaltet ist. */
  Feuchte_Regeln(HygroSoll);
  AktHygr02   = dht2.readHumidity();
  Sensor_Differenzen(AktHygr01, AktHygr02);
  Spannung = spannung_messen();
  if (Spannung < LeitSpann) {
      Serial.println("-----------------------------------------------");
      Serial.println("D E L I Q U E S Z E N Z  erreicht");
      Serial.println("-----------------------------------------------");
      Serial.println("Feuchte 1");
      Serial.println(AktHygr01);
      Serial.println("Feuchte 2");
      Serial.println(AktHygr02);
      Serial.println("Dauer");
      Serial.println(millis());
      Serial.println("Temperatur");
      Serial.println(AktTemp);
  } else {
    protokoll_schreiben(2);
  }
// Ggf. Erhöhen des Sollwertes
  if ((millis() - ZeitSoll) > IntervHygro){
    ZeitSoll = millis();
    HygroSoll = HygroSoll + IncrHygro;
    Serial.println(" ");
    Serial.println("Sollwert wurde erhöht auf ");
    Serial.print(HygroSoll);
    Serial.println(" ");
  }
}
/*---------------------------------------------------
EIGENE FUNNKTIONEN
---------------------------------------------------*/
void protokoll_schreiben(int art) {
  float laufzeit;
  laufzeit = millis() / 1000;
  if ((millis() - ZeitProt) > IntervProto ||
       art == 1) {
    Serial.println("Protokoll - aktuelle Werte");
    Serial.println("Feuchte 1");
    Serial.println(AktHygr01);
    Serial.println("Feuchte 2");
    Serial.println(AktHygr02);
    Serial.println("Temperatur");
    Serial.println(AktTemp);
    Serial.println("Spannung");
    Serial.println(Spannung);
    Serial.println("Zeit in Sekunden");
    Serial.println(laufzeit);
    ZeitProt = millis();
  } else {
//  Serial.println(art);
  }
}

float spannung_messen() {
  float sensorValue;
  float spannung;
  sensorValue = analogRead(spannungsPin); // Auslesen des Pins für die Spannungsmessung
  spannung = sensorValue * 0.0048;        // Der Arduino bekommt einen 10-Bit aufgelösten Wert, er kann also bis zu 1024 diskrete Werte liefern. Da der Arduino nur Werte zwischen 0 und 5 Volt liefern kann ist ein Teilwert gleich 5 / 1024 = 0,0048
  return spannung;
}

/*
Hier wird so lange gemessen, bis der Sollwert erreich ist
Erst dann wird die Schleife wieder verlassen. Erst dann
macht das Messen der Spannung Sinn.
*/
void Feuchte_Regeln(unsigned long Sollwert){
  boolean weiter_regeln;
  weiter_regeln = true;
  while (weiter_regeln){
    if (Sollwert < AktHygr01) {
      if (!befeuchter){
        Schalten();
      }
//
    AktHygr01   = dht1.readHumidity();
    AktHygr02   = dht2.readHumidity();
    AktTemp     = dht1.readTemperature();
//
    } else {
      weiter_regeln = false;
    }
  }
}

void Sensor_Differenzen(float wert_prim, float wert_sek){
  float diffh;
  
  diffh = wert_prim - wert_sek;            // Wenn der Wert des 2. Sensors von dem des 1. um den Wert "DifferenzSensoren" abweicht, wird eine Warnmeldung herausgegeben
  if (diffh < 0){
     diffh = diffh * -1;
  }
  if (diffh > SensorDiff) {
    if ((millis() - ZeitProt) > IntervProto){
      Serial.println("-----------------------------------------------");
      Serial.println("ACHTUNG ungleichmaessige Feuchtigkeitverteilung");
      Serial.println("-----------------------------------------------");
      ZeitProt = millis();
    }
  }
}

void Schalten(){
  digitalWrite (BefeuchterPin, HIGH);
  delay (50);
  digitalWrite (BefeuchterPin, LOW);
  befeuchter = !befeuchter;
}
