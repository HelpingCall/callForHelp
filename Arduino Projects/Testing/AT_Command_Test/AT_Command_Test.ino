<<<<<<< HEAD
// this sketch is used for testing LoNet with Arduino

// Connect VIO to +5V
// Connect GND to Ground
// Connect RX (data into SIM808) to Digital 11
// Connect TX (data out from SIM808) to Digital 10

#include <SoftwareSerial.h>

SoftwareSerial mySerial(5, 4); // RX, TX

void setup() 
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  mySerial.begin(9600);

=======
/*
 Name:    AT_Command.ino
 Created: 23.08.2020 20:17:19
 Author:  LYCRIs
*/

#include <SoftwareSerial.h>

SoftwareSerial mySerial(D2, D1); // RX, TX

void setup()
{
    // Open serial communications and wait for port to open:
    Serial.begin(115200);
    mySerial.begin(115200);
    Serial.println();
    Serial.println("Starting CFH Device");
>>>>>>> develop
}

void loop() // run over and over
{
<<<<<<< HEAD
  if (mySerial.available())
    Serial.write(mySerial.read());
   
  if (Serial.available())
  {
    while(Serial.available())
    {
      mySerial.write(Serial.read());
    }
    mySerial.println();
  }
=======
    if (mySerial.available())
        Serial.write(mySerial.read());

    if (Serial.available())
    {
        while (Serial.available())
        {
            mySerial.write(Serial.read());
        }
        mySerial.println();
    }
>>>>>>> develop
}
