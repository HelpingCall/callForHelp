#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>

#include "CFH_DeviceFunctions.h"

const char DeviceConfigurationTemplate[29] = "DeviceAlreadyConfigured:TRUE";

const String TriggerAlarmHTTPRequestLink = "http://babyyodahook.xyz/rest/arm";
const String DisarmAlarmHTTPRequestLink = "http://babyyodahook.xyz/rest/disarm";
const String RegisterDeviceHTTPRequestLink = "http://babyyodahook.xyz/rest/RegisterDevice";

const int DeviceID_Size = 37;
const int UserID_Size = 37;
const int JWT_Size = 513;
const int SSID_Size = 50;
const int Password_Size = 50;

const int DeviceID_Position = 30;
const int UserID_Position = 70;
const int JWT_position = 110;
const int SSID_position = 640;
const int Password_position = 700;

#pragma region HTTP_Functions

CFH_Structs::HTTP_Request_Struct CFH_DeviceFunctions::TestUserIDandJWT(String UserID, String JWT) //Can't be shortened as it returns a different type than boolean
{
	Serial.print("Trying to register Device: ");
	Serial.println(RegisterDeviceHTTPRequestLink);

  	HTTPClient http;
  	http.begin(RegisterDeviceHTTPRequestLink);
	http.addHeader("Content-Type", "application/json");

	int httpCode = http.POST(
	CFH_JSON::SerializeRegisterDevice(UserID, JWT));
	Serial.print("Code: ");
	Serial.println(httpCode);

	CFH_Structs::HTTP_Request_Struct JSON_HTTP_Request = {"false", ""};

	if (httpCode > 0)
	{
		Serial.println("Code > 0");
		Serial.println("Request String: ");
		Serial.println(http.getString());
		JSON_HTTP_Request = CFH_JSON::DeserializeHTTPRequestStruct(http.getString());

		http.end(); //Ends HTTP Request
		return JSON_HTTP_Request;
	}
	else
	{
		Serial.println("code < 0");
		http.end(); //Ends HTTP Request
		return JSON_HTTP_Request;
	}
}

bool CFH_DeviceFunctions::TriggerAlarm()
{	
	EEPROM.begin(786);

	String DeviceID = getDeviceIDfromEEPROM();
    Serial.println(DeviceID);

	String UserID = getUserIDfromEEPROM();
    Serial.println(UserID);

	String JWT = getJWTfromEEPROM();
    Serial.println(JWT);

  	EEPROM.end();
  	delay(1000);

	Serial.println("Trigger Alarm: ");
	CFH_Structs::GPS_Position GPS_Position = CFH_Connection::getGPS_position();
	return CFH_Connection::BooleanHTTPRequest(TriggerAlarmHTTPRequestLink, CFH_JSON::SerializeTriggerAlarm(DeviceID, UserID, JWT, GPS_Position.Latitude, GPS_Position.Longitude));

}

bool CFH_DeviceFunctions::DisarmAlarm()
{
	EEPROM.begin(786);

	String DeviceID = getDeviceIDfromEEPROM();
    Serial.println(DeviceID);

	String UserID = getUserIDfromEEPROM();
    Serial.println(UserID);

	String JWT = getJWTfromEEPROM();
    Serial.println(JWT);

  	EEPROM.end();
  	delay(1000);

	Serial.println("Disarm Alarm: ");
	return CFH_Connection::BooleanHTTPRequest(DisarmAlarmHTTPRequestLink, CFH_JSON::SerializeDisarmAlarm(DeviceID, UserID, JWT));
}

#pragma endregion


#pragma region EEPROM_Functions

float CFH_DeviceFunctions::getBatteryState()
{
  	return analogRead(A0)/(4.2*1023.0);
}


String CFH_DeviceFunctions::getDeviceIDfromEEPROM()
{
	char DeviceID[DeviceID_Size];
    EEPROM.get(DeviceID_Position, DeviceID);
	return DeviceID;
}

String CFH_DeviceFunctions::getUserIDfromEEPROM()
{
	char UserID[UserID_Size];
    EEPROM.get(UserID_Position, UserID);
	return UserID;
}

String CFH_DeviceFunctions::getJWTfromEEPROM()
{
	char JWT[JWT_Size];
    EEPROM.get(JWT_position, JWT);
	return JWT;
}

String CFH_DeviceFunctions::GetPrivateNetworkSSID()
{
	char SSID[SSID_Size];
    EEPROM.get(SSID_position, SSID);

	return SSID;
}

String CFH_DeviceFunctions::GetPrivateNetworkPassword()
{
	char Password[Password_Size];
    EEPROM.get(Password_position, Password);
	
	return Password;
}

void CFH_DeviceFunctions::writeConfigured(String JWT, String UserID, String DeviceID, String SSID, String Password)
{
	EEPROM.begin(786);

	char DeviceID_Array[37];
	DeviceID.toCharArray(DeviceID_Array, 37);

    char UserID_Array[37];
    UserID.toCharArray(UserID_Array, 37);

    char JWT_Array[513];
    JWT.toCharArray(JWT_Array, 513);

	char SSID_Array[50];
	SSID.toCharArray(SSID_Array, 50);

	char Password_Array[50];
	Password.toCharArray(Password_Array, 50);

	EEPROM.put(0, DeviceConfigurationTemplate);  // --> RegisterDevice writeConfigured();

	EEPROM.put(30, DeviceID_Array);

    EEPROM.put(70, UserID_Array);

    EEPROM.put(110, JWT_Array);

	EEPROM.put(640, SSID_Array);

	EEPROM.put(700, Password_Array);

	EEPROM.commit();


	//Testing

    Serial.print("EEPROM[0]: ");
    char dct_test[29];
    EEPROM.get(0, dct_test);
    Serial.println(dct_test);

	Serial.print("EEPROM[30]: ");
    char deviceid_test[37];
    EEPROM.get(30, deviceid_test);
    Serial.println(deviceid_test);

    Serial.print("EEPROM[70]: ");
    char userid_test[37];
    EEPROM.get(70, userid_test);
    Serial.println(userid_test);

    Serial.print("EEPROM[110]: ");
    char jwt_test[513];
    EEPROM.get(110, jwt_test);
    Serial.println(jwt_test);

    EEPROM.end();
}

bool CFH_DeviceFunctions::DeviceAlreadyConfigured()
{
	EEPROM.begin(786);
	Serial.println("Checking if Device is already configured");
	char CheckDeviceConfiguration[29];
	EEPROM.get(0, CheckDeviceConfiguration);

	Serial.print("Check Value: ");
	Serial.println(CheckDeviceConfiguration);
	EEPROM.end();
	if (String(CheckDeviceConfiguration).equals(String(DeviceConfigurationTemplate)))
	{
		Serial.println("Device Configuration confirmed!");
		return true;
	}
	else
	{
		Serial.println("Device Configuration failed!");
		return false;
	}
}


void CFH_DeviceFunctions::clearEEPROM()
{
	EEPROM.begin(786);
	Serial.println("Deleting");
	delay(2000);
	for (unsigned int i = 0; i < EEPROM.length(); i++)  //unsigned only for int and uint comparison warning
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

void CFH_DeviceFunctions::GetFullEEPROM()
{
	EEPROM.begin(786);
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

#pragma endregion