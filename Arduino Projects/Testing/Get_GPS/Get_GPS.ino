#include <SoftwareSerial.h>

SoftwareSerial Sim800l(D1,D2);  //D1 => TX     D2 => RX

bool DoOnce = true;

void setup() {
 Serial.begin(9600);
 Sim800l.begin(9600);

 delay(3000);
 Serial.println();
 Serial.println("Write AT Commands now");

 SIM800_send("AT");


 //Set Error Mode
 Serial.println(SIM800_send("AT+CMEE=2"));
 delay(1000);

 Serial.println(SIM800_send("AT+CPIN=1265"));
 delay(1000);

 //Enable Echo if not enabled by default
 Serial.println(SIM800_send("ATE1"));
 delay(1000);

 //Set the SIM800 in GPRS mode
 Serial.println(SIM800_send("AT+CGATT=1"));
 delay(1000);

 //Activate Bearer profile
 Serial.println(SIM800_send("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\" "));
 delay(1000);

 //Set APN options => 'internet.t-mobile'           https://www.hw-group.com/support/international-apn-settings-for-mobile-broadband-network-operators
 Serial.println(SIM800_send("AT+SAPBR=3,1,\"APN\",\"internet.t-mobile\" "));
 delay(2000);

 //Set USER options => 'internet.t-d1.de'
 Serial.println(SIM800_send("AT+SAPBR=3,1,\"USER\",\"internet.t-d1.de\" "));
 delay(2000);

 //Set PWD options => 't-mobile'
 Serial.println(SIM800_send("AT+SAPBR=3,1,\"PWD\",\"t-mobile\" "));
 delay(2000);

 //Open bearer Profile
 Serial.println(SIM800_send("AT+SAPBR=1,1"));
 delay(2000);

 //Get the IP address of the bearer profile
 Serial.println(SIM800_send("AT+SAPBR=2,1"));
 delay(2000);

 //Get GPS Data
 Serial.println(SIM800_send("AT+CIPGSMLOC=1,1"));
 delay(2000);
}

void loop()
{
 
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

 if(DoOnce)
 {
  
 }
}


String SIM800_send(String incoming) //Function to communicate with SIM800 module
{
    Sim800l.println(incoming); delay(100); //Print what is being sent to GSM module
    String result = "";
    while (Sim800l.available()) //Wait for result
    {
    char letter = Sim800l.read();
    result = result + String(letter); //combine char to string to get result
    }
    return result; //return the result
}
