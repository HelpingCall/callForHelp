#include <ArduinoJson.h>
#include <Arduino.h>

void setup() 
{
  Serial.begin(9600);
  Serial.println();
  Serial.println("Starting");
  Serial.println();

  
  const size_t capacity = JSON_OBJECT_SIZE(2) + 100;
  DynamicJsonDocument jsonBuffer(capacity);

  const char* json = "{\"success\":true,\"deviceID\":\"8fbf2c1a-b091-4bed-805a-1558fb49fe73\"}";

  DeserializationError error = deserializeJson(jsonBuffer, json);

  if(error)
  {
    Serial.print("deserializationJson() failed with code ");
    Serial.println(error.c_str());
    return;
  }
  bool success = jsonBuffer["success"]; // true
  const char* deviceID = jsonBuffer["deviceID"]; // "8fbf2c1a-b091-4bed-805a-1558fb49fe73"

  delay(3000);
  
  Serial.print("Reading: ");
  Serial.println(json);
  Serial.println();

  Serial.print("State: ");
  Serial.println(success);

  Serial.print("DeviceID: ");
  Serial.println(deviceID);

}

void loop() {
  // put your main code here, to run repeatedly:

}
