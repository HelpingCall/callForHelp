#ifndef __CFH_Structs__
#define __CFH_Structs__

#include <Arduino.h>

class CFH_Structs
{
public:

    CFH_Structs()
    {

    }

    #pragma region Structs

    //Contains Latitude as String and Longitude as String
	typedef struct GPS_Position
	{
		String Latitude;
		String Longitude;
	};

    //Contains Success as boolean and deviceID as String
	typedef struct HTTP_Request_Struct
	{
		boolean Success;
		String deviceID;
	};

    #pragma endregion
};
#endif