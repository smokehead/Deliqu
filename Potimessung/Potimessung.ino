float Spannung;
int Prozent;
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}
void loop() {
  // read the input on analog pin 0:
  Spannung = spannung_messen(0);
  Prozent  = umsetzen(Spannung);
  // print out the value you read:
  Serial.println(Prozent);
  delay(1000);        // delay in between reads for stability
}
//----------------------------------------------------
// Eigene Funktionen
//----------------------------------------------------
//Spannungswert ermitteln
float spannung_messen(int sPin) {
  float sensorValue;
  float spannung;
  sensorValue = analogRead(sPin);         
  spannung = sensorValue * 0.0048;        
  return spannung;
}
//Spannungswert in Einstellwert umsetzen
//bei dem 1K Poti geht 0 los, 9Uhr ist 2,17
//12Uhr 3,37 und Ende 4,24
//Der Rückgabewert soll in Stufen von 50 bis 90
//ermittelt werden.
int umsetzen (float spwert) {
if (spwert < 2.15) {
  return 50;
}
if (spwert < 3.00) {
  return 60;
}
if (spwert < 3.9) {
  return 70;
}
if (spwert < 4.2) {
  return 80;
}
return 90;

}
