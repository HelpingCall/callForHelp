#ifndef __CFH_RegisterDevice__
#define __CFH_RegisterDevice__

#include <Arduino.h>
#include <SoftwareSerial.h>

#include "CFH_Connection.h"
#include "CFH_Structs.h"


class CFH_DeviceFunctions
{
public:

	CFH_DeviceFunctions()
	{
	}

#pragma region Basic_Functions

	//Checks if the device is already configured
	static bool DeviceAlreadyConfigured();

	//Triggers the alarm for the specific device und user
	static bool TriggerAlarm();

	//Disarms the alarm for the specific device and user
	static bool DisarmAlarm();

#pragma endregion




#pragma region HTTP_Functions

	//Test ID and JWT using Website API
	static CFH_Structs::HTTP_Request_Struct TestUserIDandJWT(String UserID, String JWT);

	//Write DeviceConfigurationTemplate, JWT, UserID, DeviceID to EEPROM
	static void writeConfigured(String JWT, String UserID, String DeviceID, String SSID, String Password);

#pragma endregion




#pragma region Essential_EEPROM

	//Clear complete EEPROM
	static void clearEEPROM();

	//Prints complete EEPROM to Serial Monitor  -> maybe later website if needed for dev
	static void GetFullEEPROM();

	//Gets PrivateNetworkSSID from EEPROM
	static String GetPrivateNetworkSSID();

	//Gets privateNetworkPassword from EEPROM
	static String GetPrivateNetworkPassword();

	//Gets DeviceID from EEPROM
	static String getDeviceIDfromEEPROM();

	//GetsUserID from EEPROM
	static String getUserIDfromEEPROM();

	//Gets JWT from EEPROM
	static String getJWTfromEEPROM();

#pragma endregion




#pragma region Not_Implemented

	//Gets the current BatteryState
	static float getBatteryState();

	//To Update JWT
	static void updateJWT(String JWT);

	//To update UserID
	static void updateUserID(String UserID);

	//To update DeviceID
	static void updateDeviceID(String DeviceID);

	//Safes current Position to EEPROM     --> new header file
	static bool writeOldGPSPosition(String);

#pragma endregion


};

#endif