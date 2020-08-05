#include <Arduino.h>
#include <CFH_RegisterDevice.h>

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>

const char* ssid = "CallForHelp_Device"; // The name of the Wi-Fi network that will be created
const char* password = "schnuller";   // The password required to connect to it, leave blank for an open network

char DeviceConfigurationTemplate[29] = "DeviceAlreadyConfigured:TRUE";


bool CFH_RegisterDevice::TestUserIDandJWT(String UserID, String JWT)
{
    HTTPClient http;
    String RegisterDeviceCode = "http://babyyodahook.xyz/rest/RegisterDevice?userID=" + String(UserID) + "&jwt=" + String(JWT);
    Serial.print("Trying to register Device: ");
    Serial.println(RegisterDeviceCode);

    http.begin(RegisterDeviceCode);
    int httpCode = http.GET();
    Serial.print("Code: ");
    Serial.println(httpCode);

    if (httpCode > 0)
    {
        String httpPayload = http.getString();
        Serial.print("Text: ");
        Serial.println(httpPayload);
        if (httpPayload.equals("true"))
        {
        http.end();
        return true;
    }
    else
    {
        http.end();
        return false;
    }
  }
  else
  {
        http.end();
        return false;
  }
}

bool CFH_RegisterDevice::DeviceAlreadyConfigured()
{
  EEPROM.begin(786);
  Serial.println("Checking if Device is already configured");
  char CheckDeviceConfiguration[29];
  EEPROM.get(0, CheckDeviceConfiguration);

  Serial.print("Check Value: ");
  Serial.println(CheckDeviceConfiguration);
  EEPROM.end();
  if(String(CheckDeviceConfiguration).equals(String(DeviceConfigurationTemplate)))
  {
    Serial.println("Device Configuration confirmed!");
    DeviceConfigured = true;
    return true;
  }
  else
  {
    Serial.println("Device Configuration failed!");
    DeviceConfigured = false;
    return false;
  }
}


void CFH_RegisterDevice::clearEEPROM()
{
    EEPROM.begin(786);
    Serial.println("Deleting");
    delay(2000);
    for (unsigned int i = 0 ; i < EEPROM.length() ; i++)  //unsigned only for int and uint comparison warning
    {
        EEPROM.write(i, 0);
    }
    delay(1000);


    EEPROM.commit();
    delay(1000);
    Serial.println();
    Serial.println("Finished Deleting EEPROM.old");

    Serial.println();
    EEPROM.end();
}

void CFH_RegisterDevice::GetFullEEPROM()
{
    EEPROM.begin(786);
    Serial.print("EEPROM: ");
    for (unsigned int i = 0 ; i < EEPROM.length() ; i++) //unsigned only for int and uint comparison warning
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

