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
}

void loop() // run over and over
{
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
}
