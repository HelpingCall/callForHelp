#include <SoftwareSerial.h>
SoftwareSerial sim(5, 4);
int _timeout;
String _buffer;
String number = "+4917621323690";
void setup() {
  delay(3000); //delay for 7 seconds to make sure the modules get the signal
  Serial.begin(9600);
  Serial.println();
  _buffer.reserve(50);
  Serial.println("System Started...");
  sim.begin(9600);
  delay(1000);
}


void loop()
{
  SendMessage();
  delay(10000);
}


void SendMessage()
{
  //Serial.println ("Sending Message");
  sim.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
  //Serial.println ("Set SMS Number");
  sim.println("AT+CMGS=\"" + number + "\"\r"); //Mobile phone number to send message
  delay(1000);
  String SMS = "Sim800L test?";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  _buffer = _readSerial();
}


String _readSerial() 
{
  _timeout = 0;
  while  (!sim.available() && _timeout < 12000  )
  {
    delay(13);
    _timeout++;
  }
  if (sim.available()) {
    return sim.readString();
  }
}
