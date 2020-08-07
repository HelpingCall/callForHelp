#ifndef __CFH_JSON__
#define __CFH_JSON__

#include <Arduino.h>
#include <ArduinoJson.h>

#include <CFH_Structs.h>


class CFH_JSON
{
protected:

public:

	CFH_JSON()
	{

	}

	#pragma region Serialize

	//Serializes the trigger Alarm String -> deviceID, userID, jwt, latitude, longitude
	String SerializeTriggerAlarm(String deviceID, String userID, String jwt, String latitude, String longitude);

	//Serializes the disarm Alarm String -> deviceID, userID, jwt
	String SerializeDisarmAlarm(String deviceID, String userID, String jwt);

	//Serializes the RegisterDevice String -> userID, jwt
	String SerializeRegisterDevice(String userID, String jwt);

	#pragma endregion



	#pragma region Deserialize

	//Deserializes the trigger Alarm HTTP Request
	bool DeserializeTriggerAlarm(String jsonCode);

	//Deserializes the disarm Alarm HTTP Request
	bool DeserializeDisarmAlarm(String jsonCode);

	//Deserializes the RegisterDevice HTTP Request
	CFH_Structs::HTTP_Request_Struct DeserializeHTTPRequest(String jsonCode);

	#pragma endregion



	#pragma region Future_Functions

	//App functions for registration answer on webserver etc.


	//If the User wants to get the position of the CFH_Device -> deviceID, userID, jwt, latitude, longitude
	//String SerializePassGPS();
	//String DeserializeGetCommand();

	#pragma endregion

};

#endif