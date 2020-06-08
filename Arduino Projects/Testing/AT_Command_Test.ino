#include <SoftwareSerial.h>

SoftwareSerial Sim800l(D1,D2);  //D1 => TX   D2 => RX

void setup() {
 Serial.begin(9600);
 Sim800l.begin(9600);

  Serial.println();
 Sim800l.write("AT\r");
 Serial.write(Sim800l.read());
 Sim800l.write("AT+CMEE=2\r");
 Serial.write(Sim800l.read());
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
