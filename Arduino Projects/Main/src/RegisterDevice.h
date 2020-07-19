#ifndef __RegisterDevice__
#define __RegisterDevice__

#include <Arduino.h>

#define DEFAULT_SIM_PIN_RX 1
#define DEFAULT_SIM_PIN_TX 2
#define DEFAULT_SIM_PIN_RST 3

class RegisterDevice
{
    protected :

    public :

        unsigned int SIM_PIN_RX;
        unsigned int SIM_PIN_TX;
        unsigned int SIM_PIN_RST;

        RegisterDevice()
        {
        }
};

#endif