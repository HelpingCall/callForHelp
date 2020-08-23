
#include <Arduino.h>

#include <CFH_JSON.h>
#include <ArduinoJson.h>


#pragma region Serialize

String CFH_JSON::SerializeTriggerAlarm(String deviceID, String userID, String jwt, String latitude, String longitude)
{
	DynamicJsonDocument doc(JSON_OBJECT_SIZE(5) + 720);

	Serial.print("Set deviceID: ");
	Serial.println(doc["deviceID"].set(deviceID));

	Serial.print("Set userID: ");
	Serial.println(doc["userID"].set(userID));

	Serial.print("Set jwt: ");
	Serial.println(doc["jwt"].set(jwt));

	Serial.print("Set latitude: ");
	Serial.println(doc["latitude"].set(latitude));

	Serial.print("Set longitude: ");
	Serial.println(doc["longitude"].set(longitude));

	Serial.print("doc: ");
	serializeJson(doc, Serial);
	Serial.println();

	String json_string;
	serializeJson(doc, json_string);

	return json_string;
}

String CFH_JSON::SerializeDisarmAlarm(String deviceID, String userID, String jwt)
{
	DynamicJsonDocument doc(JSON_OBJECT_SIZE(3) + 680);

	Serial.print("Set deviceID: ");
	Serial.println(doc["deviceID"].set(deviceID));

	Serial.print("Set userID: ");
	Serial.println(doc["userID"].set(userID));

	Serial.print("Set jwt: ");
	Serial.println(doc["jwt"].set(jwt));

	Serial.print("doc: ");
	serializeJson(doc, Serial);
	Serial.println();

	String json_string;
	serializeJson(doc, json_string);

	return json_string;
}

String CFH_JSON::SerializeRegisterDevice(String userID, String jwt)
{
	DynamicJsonDocument doc(JSON_OBJECT_SIZE(2) + 630);

	Serial.print("Set userID: ");
	Serial.println(doc["userID"].set(userID));
	
	Serial.print("Set jwt: ");
	Serial.println(doc["jwt"].set(jwt));

	Serial.print("doc: ");
	serializeJson(doc, Serial);
	Serial.println();

	String json_string;
	serializeJson(doc, json_string);

	return json_string;
}

#pragma endregion



#pragma region Deserialize

bool CFH_JSON::DeserializeHTTPRequestBoolean(String jsonCode)
{
	DynamicJsonDocument doc(JSON_OBJECT_SIZE(1) + 20);

	deserializeJson(doc, jsonCode);

	return (boolean)doc["success"]; // false|true
}

CFH_Structs::HTTP_Request_Struct CFH_JSON::DeserializeHTTPRequestStruct(String jsonCode)
{
	DynamicJsonDocument doc(JSON_OBJECT_SIZE(2) + 60);

	deserializeJson(doc, jsonCode);

	CFH_Structs::HTTP_Request_Struct regStruct;

	regStruct.Success = doc["success"];
	const char* deviceID = doc["deviceID"];   //Multiple "="-Operator Error, quick fix but not perfect
	regStruct.deviceID = (String)deviceID;

	Serial.print("Deserialized success: ");
	Serial.println(regStruct.Success);
	Serial.print("Deserialized DeviceID: ");
	Serial.println(regStruct.deviceID);

	return regStruct;
}

#pragma endregion