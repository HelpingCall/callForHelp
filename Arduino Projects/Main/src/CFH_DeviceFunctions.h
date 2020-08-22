#ifndef __CFH_RegisterDevice__
#define __CFH_RegisterDevice__

#include <Arduino.h>
#include <SoftwareSerial.h>

#include <CFH_JSON.h>
#include <CFH_Connection.h>
#include <CFH_Structs.h>

SoftwareSerial Sim800lStream(D1,D2);  //D1 => TX     D2 => RX

CFH_Connection Connection_Instance(Sim800lStream);
CFH_JSON JSON_Instance;

class CFH_DeviceFunctions
{
public:

	bool DeviceConfigured = false;

	CFH_DeviceFunctions()
	{
	}

#pragma region Basic_Functions

	//Checks if the device is already configured
	bool DeviceAlreadyConfigured();

	//Triggers the alarm for the specific device und user
	bool TriggerAlarm();

	//Disarms the alarm for the specific device and user
	bool DisarmAlarm();

#pragma endregion




#pragma region HTTP_Functions

	//Test ID and JWT using Website API
	CFH_Structs::HTTP_Request_Struct TestUserIDandJWT(String UserID, String JWT);

	//Write DeviceConfigurationTemplate, JWT, UserID, DeviceID to EEPROM
	void writeConfigured(String JWT, String UserID, String DeviceID);

#pragma endregion




#pragma region Essential_EEPROM

	//Clear complete EEPROM
	void clearEEPROM();

	//Prints complete EEPROM to Serial Monitor  -> maybe later website if needed for dev
	void GetFullEEPROM();

#pragma endregion




#pragma region Not_Implemented

	//To Update JWT
	void updateJWT(String JWT);

	//To update UserID
	void updateUserID(String UserID);

	//To update DeviceID
	void updateDeviceID(String DeviceID);

	//Safes current Position to EEPROM     --> new header file
	bool writeOldGPSPosition(String);

#pragma endregion


};

#endif