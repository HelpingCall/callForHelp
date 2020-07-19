#ifndef __CFHWifi__
#define __CFHWifi__

#include <Arduino.h>

#define DEFAULT_SIM_PIN_RX 1
#define DEFAULT_SIM_PIN_TX 2
#define DEFAULT_SIM_PIN_RST 3

class CFHWifi
{
    protected :

    public :

        unsigned int SIM_PIN_RX;
        unsigned int SIM_PIN_TX;
        unsigned int SIM_PIN_RST;

        CFHWifi(Stream& SerialStream)
        {
            SIM_PIN_RX = DEFAULT_SIM_PIN_RX;
            SIM_PIN_TX = DEFAULT_SIM_PIN_TX;
            SIM_PIN_RST = DEFAULT_SIM_PIN_RST;
        }

        CFHWifi(Stream& SerialStream, uint SIM_RX, uint SIM_TX)
        {
            SIM_PIN_RX = SIM_RX;
            SIM_PIN_TX = SIM_TX;
            SIM_PIN_RST = DEFAULT_SIM_PIN_RST;
        }

        CFHWifi(Stream& SerialStream, uint SIM_RX, uint SIM_TX, uint SIM_RST)
        {
            SIM_PIN_RX = SIM_RX;
            SIM_PIN_TX = SIM_TX;
            SIM_PIN_RST = SIM_RST;
        } 
};

#endif