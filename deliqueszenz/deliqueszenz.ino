#include "DHT.h"                          
#define DHTPIN1 3                               
#define DHTPIN2 2
#define DHTTYPE DHT22                    

unsigned long IntervProto;
unsigned long IntervHygro;
float LeitSpann;
int IncrHygro;
float SensorDiff;
int spannungsPin0;
int spannungsPin1;
int spannungsPin2;
int spannungsPin3;
int BefeuchterPin;
DHT dht1(DHTPIN1, DHTTYPE);    
DHT dht2(DHTPIN2, DHTTYPE);    
unsigned long ZeitProt;
unsigned long ZeitSoll;
unsigned long ZeitHilf;
float AktHygr01;
float AktHygr02;
float AktTemp01;
float AktTemp02;
int SensorValue;
int Spannung0;
int Spannung1;
int Spannung2;
int Spannung3;
unsigned long HygroSoll;
boolean Probe1, Probe2, Probe3, Probe4 = false;     


void setup() {
  IntervProto   = 20000;         
  IntervHygro   = 600000;       
  LeitSpann     = 2.43;
  IncrHygro     = 5;
  HygroSoll     = 90;            
  SensorDiff    = 3;
  spannungsPin0  = 0;
  spannungsPin1  = 1;
  spannungsPin2  = 2;
  spannungsPin3  = 3;
  BefeuchterPin = 7;
  
  ZeitProt = millis();
  ZeitSoll = millis();
  ZeitHilf = millis();

  Serial.begin(9600);
  Serial.println("rh1\trh2\tC1\tC2\tV1\tV2\tV3\tV4\tBS\t1\t2\t3\t4\ts");

  SensorValue = 0;

  AktHygr01   = dht1.readHumidity();
  AktHygr02   = dht2.readHumidity();
  AktTemp01   = dht1.readTemperature();
  AktTemp02   = dht2.readTemperature();

  if (isnan(AktHygr01) ||
     isnan(AktHygr02)) {
     Serial.println("Die Feuchtigkeit konnte nicht gelesen werden");
  }
  Spannung0 = digitalRead(spannungsPin0);
  Spannung1 = digitalRead(spannungsPin1);
  Spannung2 = digitalRead(spannungsPin2);
  Spannung3 = digitalRead(spannungsPin3);
  
  protokoll_schreiben(1);
  
}

void loop() {
  Feuchte_Regeln(HygroSoll);

  AktHygr01   = dht1.readHumidity();
  AktHygr02   = dht2.readHumidity();
  AktTemp01   = dht1.readTemperature();
  AktTemp02   = dht2.readTemperature();
  
  if (!Probe1){
  Spannung0 = digitalRead(spannungsPin0);
  }
  if (!Probe2) {
  Spannung1 = digitalRead(spannungsPin1);
  }
  if (!Probe3) {
  Spannung2 = digitalRead(spannungsPin2);
  }
  if (!Probe4) {
  Spannung3 = digitalRead(spannungsPin3);
  }
 //
  if (Spannung0 == HIGH || 
      Spannung1 == HIGH ||
      Spannung2 == HIGH ||
      Spannung3 == HIGH) {
   Deliqueszenz_erreicht();
   delay(60000);
} else {
    protokoll_schreiben(2);
  }
// Ggf. Erhöhen des Sollwertes
  if ((millis() - ZeitSoll) > IntervHygro){
    ZeitSoll = millis();
    HygroSoll = HygroSoll + IncrHygro;
    Serial.println("Sollwert wurde erhöht auf ");
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
    Serial.print(AktHygr01);
    Serial.print("\t");
    Serial.print(AktHygr02);
    Serial.print("\t");
    Serial.print(AktTemp01);
    Serial.print("\t");
    Serial.print(AktTemp02);
    Serial.print("\t");
    Serial.print(Spannung0);
    Serial.print("\t");
    Serial.print(Spannung1);
    Serial.print("\t");
    Serial.print(Spannung2);
    Serial.print("\t");
    Serial.print(Spannung3);
    Serial.print("\t");
    Serial.print(Probe1);
    Serial.print("\t");
    Serial.print(Probe2);
    Serial.print("\t");
    Serial.print(Probe3);
    Serial.print("\t");
    Serial.print(Probe4);
    Serial.print("\t");
    Serial.println(laufzeit);
    ZeitProt = millis();
}
}

float spannung_messen(int sPin) {
  float sensorValue;
  float spannung;
  sensorValue = analogRead(sPin);         
  spannung = sensorValue * 0.0048;        
  return spannung;
}


void Feuchte_Regeln(unsigned long Sollwert){
  boolean weiter_regeln;
  weiter_regeln = true;
  while (weiter_regeln){
    protokoll_schreiben(2);
    if (Sollwert > AktHygr01) {
      digitalWrite(BefeuchterPin, HIGH);
//
    AktHygr01   = dht1.readHumidity();
    AktHygr02   = dht2.readHumidity();
    AktTemp01   = dht1.readTemperature();
    AktTemp02   = dht2.readTemperature();
//
    } else {
        digitalWrite(BefeuchterPin, LOW);
      weiter_regeln = false;
    }
  }
}

void Deliqueszenz_erreicht(){
      Serial.println("-----------------------------------------------");
      if (Spannung0 == HIGH && !Probe1) {
        Probe1 = true;
        Serial.println("P R O B E   1   hat");
      }
      if (Spannung1 == HIGH && !Probe2) {
        Probe2 = true;
        Serial.println("P R O B E   2   hat");
      }
      if (Spannung2 == HIGH && !Probe3) {
        Probe3 = true;
        Serial.println("P R O B E   3   hat");
      }
      if (Spannung3 == HIGH && !Probe4) {
        Probe4 = true;
        Serial.println("P R O B E   4   hat");
      }
      Serial.println("D E L I Q U E S Z E N Z  erreicht");
      Serial.println("-----------------------------------------------");
      Serial.println("Feuchte 1");
      Serial.println(AktHygr01);
      Serial.println("Feuchte 2");
      Serial.println(AktHygr02);
      Serial.println("Dauer");
      Serial.println(millis());
      Serial.println("Temperatur 1");
      Serial.println(AktTemp01);
      Serial.println("-----------------------------------------------");
      if (Spannung0 == HIGH && !Probe1) {
        Probe1 = true;
        Serial.println("P R O B E   1   hat");
      }
      if (Spannung1 == HIGH && !Probe2) {
        Probe2 = true;
        Serial.println("P R O B E   2   hat");
      }
      if (Spannung2 == HIGH && !Probe3) {
        Probe3 = true;
        Serial.println("P R O B E   3   hat");
      }
      if (Spannung3 == HIGH && !Probe4) {
        Probe4 = true;
        Serial.println("P R O B E   4   hat");
      }
      Serial.println("D E L I Q U E S Z E N Z  erreicht");
      Serial.println("-----------------------------------------------");
      Serial.println("Feuchte 1");
      Serial.println(AktHygr01);
      Serial.println("Feuchte 2");
      Serial.println(AktHygr02);
      Serial.println("Dauer");
      Serial.println(millis());
      Serial.println("Temperatur 1");
      Serial.println(AktTemp01);
}
