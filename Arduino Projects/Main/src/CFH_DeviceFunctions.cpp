#include <Arduino.h>
#include <CFH_DeviceFunctions.h>

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>

const char* ssid = "CallForHelp_Device"; // The name of the Wi-Fi network that will be created
const char* password = "schnuller";   // The password required to connect to it, leave blank for an open network

char DeviceConfigurationTemplate[29] = "DeviceAlreadyConfigured:TRUE";



#pragma region HTTP_Functions

CFH_Structs::HTTP_Request_Struct CFH_DeviceFunctions::TestUserIDandJWT(String UserID, String JWT)
{
	HTTPClient http;
	CFH_Structs::HTTP_Request_Struct JSON_HTTP_Request = {"false", ""};
	String RegisterDeviceCode = "http://babyyodahook.xyz/rest/RegisterDevice?json=" + JSON_Instance.SerializeRegisterDevice(UserID, JWT);
	Serial.print("Trying to register Device: ");
	Serial.println(RegisterDeviceCode);

	http.begin(RegisterDeviceCode);   // old version without client
	int httpCode = http.GET();
	Serial.print("Code: ");
	Serial.println(httpCode);

	
	if (httpCode > 0)
	{
		JSON_HTTP_Request = JSON_Instance.DeserializeHTTPRequest(http.getString());

		Serial.print("Success: ");
		Serial.println(JSON_HTTP_Request.Success);

		http.end(); //Ends HTTP Request
		return JSON_HTTP_Request;
	}
	else
	{
		http.end(); //Ends HTTP Request
		return JSON_HTTP_Request;
	}
}

void CFH_DeviceFunctions::writeConfigured(String JWT, String UserID, String DeviceID)
{
	EEPROM.begin(786);

	char DeviceID_Array[37];
	DeviceID.toCharArray(DeviceID_Array, 37);

    char UserID_Array[37];
    UserID.toCharArray(UserID_Array, 37);

    char JWT_Array[513];
    JWT.toCharArray(JWT_Array, 513);

	EEPROM.put(0, DeviceConfigurationTemplate);  // --> RegisterDevice writeConfigured();

	EEPROM.put(30, DeviceID_Array);

    EEPROM.put(70, UserID_Array);

    EEPROM.put(110, JWT_Array);

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

    EEPROM.commit();
    EEPROM.end();
}

#pragma endregion



#pragma region Basic_Functions

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

bool CFH_DeviceFunctions::TriggerAlarm(String latitude, String longitude)
{
	EEPROM.begin(786);

	Serial.print("EEPROM[30]: ");
    char DeviceID[37];
    EEPROM.get(30, DeviceID);
    Serial.println(DeviceID);

    Serial.print("EEPROM[70]: ");
    char UserID[37];
    EEPROM.get(70, UserID);
    Serial.println(UserID);

    Serial.print("EEPROM[110]: ");
    char JWT[513];
    EEPROM.get(110, JWT);
    Serial.println(JWT);

  	EEPROM.end();
  	delay(1000);

  	String ArmDeviceLink = "http://babyyodahook.xyz/rest/arm?json=" +JSON_Instance.SerializeTriggerAlarm(DeviceID, UserID, JWT, latitude, longitude);

  	Serial.print("ArmString: ");
  	Serial.println(ArmDeviceLink);

  	HTTPClient http;

  	http.begin(ArmDeviceLink);
	int httpCode = http.GET();
	Serial.print("Code: ");
	Serial.println(httpCode);

	CFH_Structs::HTTP_Request_Struct JSON_HTTP_Request = {"false", ""};

	if (httpCode > 0)
	{
		JSON_HTTP_Request = JSON_Instance.DeserializeHTTPRequest(http.getString());

		if(JSON_HTTP_Request.Success)
		{
			Serial.println("Alarm triggered");
			http.end(); //Ends HTTP Request
			return true;
		}
		else
		{
			Serial.println("Alarm not triggered");
			http.end(); //Ends HTTP Request
			return false;
		}
	}
	else
	{
		Serial.println("Alarm not triggered");
		http.end(); //Ends HTTP Request
		return false;
	}
}

bool CFH_DeviceFunctions::DisarmAlarm()
{
	EEPROM.begin(786);

	Serial.print("EEPROM[30]: ");
    char DeviceID[37];
    EEPROM.get(30, DeviceID);
    Serial.println(DeviceID);

    Serial.print("EEPROM[70]: ");
    char UserID[37];
    EEPROM.get(70, UserID);
    Serial.println(UserID);

    Serial.print("EEPROM[110]: ");
    char JWT[513];
    EEPROM.get(110, JWT);
    Serial.println(JWT);

  	EEPROM.end();
  	delay(1000);

  	String DisrmDeviceLink = "http://babyyodahook.xyz/rest/arm?json=" +JSON_Instance.SerializeDisarmAlarm(DeviceID, UserID, JWT);

  	Serial.print("DisarmString: ");
  	Serial.println(DisrmDeviceLink);

  	HTTPClient http;

  	http.begin(DisrmDeviceLink);
	int httpCode = http.GET();
	Serial.print("Code: ");
	Serial.println(httpCode);

	CFH_Structs::HTTP_Request_Struct JSON_HTTP_Request = {"false", ""};

	if (httpCode > 0)
	{
		JSON_HTTP_Request = JSON_Instance.DeserializeHTTPRequest(http.getString());

		if(JSON_HTTP_Request.Success)
		{
			Serial.println("Alarm disarmed");
			http.end(); //Ends HTTP Request
			return true;
		}
		else
		{
			Serial.println("Alarm not disarmed");
			http.end(); //Ends HTTP Request
			return false;
		}
	}
	else
	{
		Serial.println("Alarm not disarmed");
		http.end(); //Ends HTTP Request
		return false;
	}
}

#pragma endregion



#pragma region Essential_EEPROM

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



//EEPROM Zugriff Methoden für die genauen Bereiche -> writeJWT(String JWT)  writeUserID(String UserID)    writeDeviceID(String DeviceID) writeConfigured()
          // -->  ggf. update Funktionen welche dann durch die App aufgerufen werden sofern der Reset Knopf für so und so lange gedrückt wird -> kann Per App dann resettet werden auf Werkseinstellungen