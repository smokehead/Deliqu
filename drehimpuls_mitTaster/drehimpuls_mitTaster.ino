/*
Über die Encoder Library. http://www.pjrc.com/td_libs_Encoder.htm
Es werden 2 Drehimpulsgeber über die Pins 2 (Interrupt 0) und
4 sowie 3(Interrupt 1) und 5 ausgewertet.
Es wird einfach ein Zähler Hoch- oder Heruntergezählt
und auf dem Serial Monitor ausgegeben.
Pro Raster wird der Zähler 4 mal erhöht ! Und es werden 4
Impule ausgelöst.
*/

#include "Encoder.h"
//
int taster = 7;
int tasterStat;
int tasterAlt = LOW;
int zaehler = 0;
Encoder knobleft(2, 4);
//Encoder knobright(3, 5);
//
long posleft  = -999;
long posright = -999;

void setup(){
  Serial.begin(9600);
  Serial.println("Testprogramm f. Drehimpulsgeber");
}

void loop(){
  long newLeft, newRight;
  newLeft  = knobleft.read();
//  newRight = knobright.read();
//
//  if (newLeft != posleft || newRight != posright){
  if (newLeft != posleft){
    Serial.print("Links = ");
    Serial.print(newLeft);
//    Serial.print(" , Rechts  = ");
//    Serial.print(newRight);
    Serial.println();
    posleft  = newLeft;
//    posright = newRight;
    zaehler++;
  }
  if (Serial.available()){
    Serial.read();
    Serial.println("Reset durchgefuehrt");
    knobleft.write(0);
//    knobright.write(0);
  }
  tasterStat = digitalRead(taster);
  if (tasterStat == HIGH){
    if (tasterAlt == LOW){
      tasterAlt = HIGH;
      Serial.println("Taster gedrueckt");
      delay(150);
    }
  } else {
    tasterAlt = LOW;
  }
}
