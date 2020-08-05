#ifndef __CFH_JSON__
#define __CFH_JSON__

#include <Arduino.h>
#include <ArduinoJson.h>

class CFH_JSON
{
    protected :

    public :

        CFH_JSON()
        {

        }
        
        //Structs

        //Contains Success as boolean and deviceID as String
        struct RegisterDeviceStruct 
        {
            boolean Success;
            String deviceID;
        };


        //Curent Functions

        //Serializes the trigger Alarm String -> deviceID, userID, jwt, latitude, longitude
        String SerializeTriggerAlarm(String deviceID, String userID, String jwt, String latitude, String longitude);

        //Serializes the disarm Alarm String -> deviceID, userID, jwt
        String SerializeDisarmAlarm(String deviceID, String userID, String jwt);

        //Serializes the RegisterDevice String -> userID, jwt
        String SerializeRegisterDevice(String userID, String jwt);


        //Deserializes the trigger Alarm HTTP Request
        bool DeserializeTriggerAlarm(String jsonCode);

        //Deserializes the disarm Alarm HTTP Request
        bool DeserializeDisarmAlarm(String jsonCode);

        //Deserializes the RegisterDevice HTTP Request
        RegisterDeviceStruct DeserializeRegisterDevice(String jsonCode);



        //Future Functions
        
        //If the User wants to get the position of the CFH_Device -> deviceID, userID, jwt, latitude, longitude
        //String SerializePassGPS();
        //String DeserializeGetCommand();

};

#endif