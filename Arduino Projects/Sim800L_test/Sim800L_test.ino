#include <GSMSim.h>
#include <SoftwareSerial.h>
#define RX 12 //festlegen des RX-Pin am Arduino
#define TX 14 //festlegen des TX-Pin am Arduino
GSMSim gsm(RX, TX); //Ruft die Bibliothek auf

void setup()
{
 gsm.start(); //Startet die Funktionen der Bibliothek
 Serial.begin(115200); //Startet die Verbindung zum PC
 while (!Serial){ //auf den Seriellen Monitor warten
 }
 Serial.print("Status: ");
 Serial.println(gsm.pinStatus());
 Serial.println("Sending");
 gsm.smsTextMode(true); //aktiviert den SMS modus des Moduls
 char* Nummer = "+4917621323690"; //gibt die Nummer an
 char* Nachricht = "Hello World!"; //legt die Nachricht fest
 //Senden der Nachricht an die angegebene Nummmer:
 gsm.smsSend(Nummer, Nachricht);

}
void loop()
{
 // put your main code here, to run repeatedly:
} 
