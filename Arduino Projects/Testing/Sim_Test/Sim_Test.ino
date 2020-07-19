#include <GSMSimSMS.h>
#include <SoftwareSerial.h>


#define RESET_PIN 10 // you can use any pin.

static volatile int num = 0;


#define DEBUG 1
#define SIM800_TX_PIN D1 // TX pin of SIM800L
#define SIM800_RX_PIN D2 // RX pin of SIM800L

SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);

GSMSimSMS sms(serialSIM800, RESET_PIN); // GSMSimSMS inherit from GSMSim. You can use GSMSim methods with it.

void setup() {
  serialSIM800.begin(115200); // If you dont change module baudrate, it comes with auto baudrate.

  while(!serialSIM800) {
    ; // wait for module for connect.
  }

  Serial.begin(115200); // Serial for debug...

  delay(7000);
  // Init module...
  sms.init(); // use for init module. Use it if you dont have any valid reason.
  Serial.println();
  Serial.println();

  
  Serial.print("Set Phone Function... ");
  Serial.println(sms.setPhoneFunc(1));
  delay(2000);

  Serial.print("Unlock Pin... ");
  Serial.println(sms.enterPinCode("1265"));
  delay(2000);
  
  Serial.print("is Sim inserted?... ");
  Serial.println(sms.isSimInserted());
  delay(2000);

  Serial.print("is Module Registered to Network?... ");
  Serial.println(sms.isRegistered());
  delay(2000);

  Serial.print("Signal Quality... ");
  Serial.println(sms.signalQuality());
  delay(2000);

  Serial.print("Operator Name... ");
  Serial.println(sms.operatorNameFromSim());
  delay(2000);



  Serial.print("Init SMS... ");
  Serial.println(sms.initSMS()); // Its optional but highly recommended. Some function work with this function.
  delay(2000);

  Serial.print("List Unread SMS... ");
  Serial.println(sms.list(true)); // Its optional but highly recommended. Some function work with this function.
  delay(2000);

  
  Serial.print("SMS to any number... ");
  Serial.println(sms.send("+4917621323690", "Call For Help Test")); // only use ascii chars please
  delay(8000);

  // For other methods please look at readme.txt file.

  Serial.println("Begin to listen incoming messages...");

}

void loop() {
  
  // Use your Serial interface...
  if(serialSIM800.available()) {
      String buffer = "";
      buffer = serialSIM800.readString();
      num = num + 1;
      Serial.print(num);
      Serial.print(". ");
      //Serial.println(buffer);

      /**/
      // This example for how you catch incoming calls.
      if(buffer.indexOf("+CMTI:") != -1) {
        
        Serial.print("SMS Index No... ");
        int indexno = sms.indexFromSerial(buffer);
        Serial.println(indexno);

        Serial.print("Who send the message?...");
        Serial.println(sms.getSenderNo(indexno));

        Serial.print("Read the message... ");
        Serial.println(sms.readFromSerial(buffer));
      } else {
        Serial.println(buffer);
      }
      
  }
  
  // put your main code here, to run repeatedly:
}
