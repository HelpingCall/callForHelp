#ifndef __CFH_Connection__
#define __CFH_Connection__

#include <Arduino.h>

#include "CFH_Structs.h"
#include "CFH_JSON.h"
#include "SoftwareSerial.h"

#define DEFAULT_SIM_PIN_RX 1
#define DEFAULT_SIM_PIN_TX 2
#define DEFAULT_SIM_PIN_RST 3

#define TIME_OUT_READ_SERIAL 5000


static SoftwareSerial Sim800lStream(D1, D2);  //D1 => TX     D2 => RX

class CFH_Connection
{

    protected:
        static void _readSerial();
        static void _readSerial(uint32_t timeout);
        static String SIM800_send(String incoming);
    public:
        unsigned int SIM_PIN_RX = DEFAULT_SIM_PIN_RX;
        unsigned int SIM_PIN_TX = DEFAULT_SIM_PIN_TX;
        unsigned int SIM_PIN_RST = DEFAULT_SIM_PIN_RST;

        CFH_Connection()
        {
        }

        //Returns Struct containing latitude and longitude
        static CFH_Structs::GPS_Position getGPS_position();

        //Starts the HTTP Request itself
        static bool BooleanHTTPRequest(String link, String JSON_String);

        static unsigned int _timeout;
        static String _buffer;
};

#endif
