#include <CFH_JSON.h>

#include <Arduino.h>
#include <ArduinoJson.h>


#pragma region Serialize

String CFH_JSON::SerializeTriggerAlarm(String deviceID, String userID, String jwt, String latitude, String longitude)
{
	DynamicJsonDocument doc(JSON_OBJECT_SIZE(5));

	doc["deviceID"] = deviceID;
	doc["userID"] = userID;
	doc["jwt"] = jwt;
	doc["latitude"] = latitude;
	doc["longitude"] = longitude;

	serializeJson(doc, Serial);

	String json_string;
	serializeJson(doc, json_string);

	return json_string;
}

String CFH_JSON::SerializeDisarmAlarm(String deviceID, String userID, String jwt)
{
	DynamicJsonDocument doc(JSON_OBJECT_SIZE(3));

	doc["deviceID"] = deviceID;
	doc["userID"] = userID;
	doc["jwt"] = jwt;

	serializeJson(doc, Serial);

	String json_string;
	serializeJson(doc, json_string);

	return json_string;
}

String CFH_JSON::SerializeRegisterDevice(String userID, String jwt)
{
	DynamicJsonDocument doc(JSON_OBJECT_SIZE(2));

	doc["userID"] = userID;
	doc["jwt"] = jwt;

	serializeJson(doc, Serial);

	String json_string;
	serializeJson(doc, json_string);

	return json_string;
}

#pragma endregion

int test = 0;

#pragma region Deserialize

bool CFH_JSON::DeserializeTriggerAlarm(String jsonCode)
{
	DynamicJsonDocument doc(JSON_OBJECT_SIZE(1) + 20);

	deserializeJson(doc, jsonCode);

	return (boolean)doc["success"]; // false|true
}

bool CFH_JSON::DeserializeDisarmAlarm(String jsonCode)
{
	DynamicJsonDocument doc(JSON_OBJECT_SIZE(1) + 20);

	deserializeJson(doc, jsonCode);

	return (boolean)doc["success"]; // false|true
}

CFH_Structs::HTTP_Request_Struct CFH_JSON::DeserializeHTTPRequest(String jsonCode)
{
	DynamicJsonDocument doc(JSON_OBJECT_SIZE(2) + 60);

	deserializeJson(doc, jsonCode);

	CFH_Structs::HTTP_Request_Struct regStruct;

	regStruct.Success = doc["success"];
	const char* deviceID = doc["deviceID"];   //Multiple "="-Operator Error, quick fix but not perfect
	regStruct.deviceID = (String)deviceID;

	Serial.print("success: ");
	Serial.println(regStruct.Success);
	Serial.print("DeviceID: ");
	Serial.println(regStruct.deviceID);

	return regStruct;
}

#pragma endregion