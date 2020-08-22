#ifndef __CFH_Connection__
#define __CFH_Connection__

#include <Arduino.h>

#include <CFH_Structs.h>
#include <CFH_JSON.h>

#define DEFAULT_SIM_PIN_RX 1
#define DEFAULT_SIM_PIN_TX 2
#define DEFAULT_SIM_PIN_RST 3


CFH_JSON JSON_Connection_Instance;

#define TIME_OUT_READ_SERIAL 5000

class CFH_Connection
{

    protected:
        Stream& SimStream;
        unsigned int _timeout;
        String _buffer;
        void _readSerial();
        void _readSerial(uint32_t timeout);
        String SIM800_send(String incoming);

    public:
        unsigned int SIM_PIN_RX;
        unsigned int SIM_PIN_TX;
        unsigned int SIM_PIN_RST;

        CFH_Connection(Stream& SerialStream) : SimStream(SerialStream)
        {
            SIM_PIN_RX = DEFAULT_SIM_PIN_RX;
            SIM_PIN_TX = DEFAULT_SIM_PIN_TX;
            SIM_PIN_RST = DEFAULT_SIM_PIN_RST;
        }

        CFH_Connection(Stream& SerialStream, uint SIM_RX, uint SIM_TX) : SimStream(SerialStream)
        {
            SIM_PIN_RX = SIM_RX;
            SIM_PIN_TX = SIM_TX;
            SIM_PIN_RST = DEFAULT_SIM_PIN_RST;
        }

        CFH_Connection(Stream& SerialStream, uint SIM_RX, uint SIM_TX, uint SIM_RST) : SimStream(SerialStream)
        {
            SIM_PIN_RX = SIM_RX;
            SIM_PIN_TX = SIM_TX;
            SIM_PIN_RST = SIM_RST;
        }

        //Returns Struct containing latitude and longitude
        CFH_Structs::GPS_Position getGPS_position();

        //Starts the HTTP Request itself
        bool BooleanHTTPRequest(String link, String JSON_String);

};

#endif
