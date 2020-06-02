#include <EEPROM.h>
#include <Arduino.h>

void setup()
{
  Serial.begin(9600);
  EEPROM.begin(1024);
  Serial.println();
  Serial.print("EEPROM.old: ");
  for (int i = 0 ; i < EEPROM.length() ; i++) 
  {
    Serial.print("[");
    Serial.print(i);
    Serial.print("]");
    Serial.print(char(int(EEPROM.read(i))));
    Serial.print(" : ");
    delay(5);
  }
  Serial.println();
  Serial.print("Deleting");
  delay(2000);
  for (int i = 0 ; i < EEPROM.length() ; i++) 
  {
    EEPROM.write(i, 0);
    Serial.print(".");
    delay(5);
  }
  delay(1000);
  EEPROM.commit();
  Serial.println();
  Serial.println("Finished Deleting EEPROM.old");
  
  Serial.println();
  Serial.print("EEPROM.new:");
  for (int i = 0 ; i < EEPROM.length() ; i++) 
  {
    Serial.print("[");
    Serial.print(i);
    Serial.print("]");
    Serial.print(char(int(EEPROM.read(i))));
    Serial.print(" : ");
    delay(5);
  }
  Serial.println();
}
void loop()
{
  
}
