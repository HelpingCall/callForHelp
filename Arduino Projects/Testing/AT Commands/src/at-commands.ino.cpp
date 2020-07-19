# 1 "C:\\Users\\LYCRIs\\AppData\\Local\\Temp\\tmpmfikgcqd"
#include <Arduino.h>
# 1 "D:/Windows Ordner/OneDrive/TH-DEV/Digitaler Hilferuf/CallForHelp-Embedded/Arduino Projects/Testing/AT Commands/src/at-commands.ino"
#include <SoftwareSerial.h>

SoftwareSerial Sim800l(D2,D1);
void setup();
void loop();
#line 5 "D:/Windows Ordner/OneDrive/TH-DEV/Digitaler Hilferuf/CallForHelp-Embedded/Arduino Projects/Testing/AT Commands/src/at-commands.ino"
void setup() {
 Serial.begin(9600);
 Sim800l.begin(9600);

 Serial.println();
 Serial.println();
 Serial.println("Write AT Commands now");

}

void loop() {

 while(Sim800l.available())
 {
  Serial.write(Sim800l.read());
 }

 while(Serial.available())
 {
  char inc = Serial.read();
  if(inc == '$')
  {
    Sim800l.write(char(26));
  }
  else
  {
    Sim800l.write(inc);
  }
 }
}