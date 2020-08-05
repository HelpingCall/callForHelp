#ifndef __CFH_RegisterDevice__
#define __CFH_RegisterDevice__

#include <Arduino.h>
#include <CFH_JSON.h>
class CFH_RegisterDevice
{
    protected :

    public :


        bool DeviceConfigured = false;

        CFH_JSON json;
        
        CFH_RegisterDevice()
        {

        }

        //Test ID and JWT using Website API
        bool TestUserIDandJWT(String UserID, String JWT);
    	
        //Clear complete EEPROM
        void clearEEPROM();

        //Prints complete EEPROM to Serial Monitor  -> maybe later website if needed for dev
        void GetFullEEPROM();

        //Checks if the device is already configured
        bool DeviceAlreadyConfigured();
};

#endif