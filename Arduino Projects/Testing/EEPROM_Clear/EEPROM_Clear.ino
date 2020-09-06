/*
   EEPROM Write

   Stores values read from analog input 0 into the EEPROM.
   These values will stay in the EEPROM when the board is
   turned off and may be retrieved later by another sketch.
*/

#include <EEPROM.h>

// the current address in the EEPROM (i.e. which byte
// we're going to write to next)

void setup()
{
  Serial.begin(115200);
  GetFullEEPROM();

  Serial.println("Deleting in 2s");
  delay(2000);
  clearEEPROM();
  delay(2000);
  GetFullEEPROM();
  
}

void loop() 
{
  
}


void clearEEPROM()
{
  EEPROM.begin(1000);
  Serial.println("Deleting");
  delay(2000);
  for (unsigned int i = 0; i < EEPROM.length(); i++)  //unsigned only for int and uint comparison warning
  {
    EEPROM.write(i, 0);
    if(i%100 == 0)
    {
      Serial.println(i);
    }
  }
  delay(1000);


  EEPROM.commit();
  delay(1000);
  Serial.println();
  Serial.println("Finished Deleting EEPROM.old");

  Serial.println();
  EEPROM.end();
}

void GetFullEEPROM()
{
  EEPROM.begin(1000);
  Serial.print("EEPROM: ");
  for (unsigned int i = 0; i < EEPROM.length(); i++) //unsigned only for int and uint comparison warning
  {
    Serial.print("[");
    Serial.print(i);
    Serial.print("]");
    Serial.print(char(int(EEPROM.read(i))));
    Serial.println("\t");
    delay(5);
  }
  Serial.println();
  EEPROM.end();
}
