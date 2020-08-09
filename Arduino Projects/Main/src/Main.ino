#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <CFH_DeviceFunctions.h>
#include <CFH_Connection.h>

const char* ssid = "CallForHelp_Device"; // The name of the Wi-Fi network that will be created
const char* password = "schnuller";   // The password required to connect to it, leave blank for an open network
const char* CFH_DNS_Name = "CFH_Device";

#define LED_PIN    5  // Pin für die LED Data
#define NUM_LEDS   2  // Anzahl der LEDs zur Anzeige vom Status etc.
#define SWITCH_PIN D0  // Pin des Schalters

//#define Sim_TX D1 // TX Pin des Sim Moduls
//#define Sim_RX D2 // RX Pin des Sim Moduls

bool DeviceConfigured = false;
int oldStatus = 0;
CRGB leds[NUM_LEDS];

//TODO: DNS Server aktivieren für CallForHelp

ESP8266WebServer CFHWebServer(80);
CFH_DeviceFunctions CFH_Device;

//String NetworkSSID = "FRITZ!Box 7530 UM";   // ggf. = CFHWebServer.arg("NetworkSSID");
//String NetworkPassword = "Anke1209"; // ggf. = CFHWebServer.arg("NetworkPassword");

String NetworkSSID = "Magnus";   // ggf. = CFHWebServer.arg("NetworkSSID");
String NetworkPassword = "Schnuller"; // ggf. = CFHWebServer.arg("NetworkPassword");


#pragma region next to work on

//setup which starts at each start of the device
void setup()
{
	Serial.begin(115200);
	Serial.println();
	Serial.println("ESP8266 Started");
	pinMode(SWITCH_PIN, INPUT_PULLDOWN_16);
	FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
	FastLED.setBrightness(20);

	leds[0] = CRGB(255, 255, 255);
	leds[1] = CRGB(0,0,255);
	FastLED.show();

	delay(4000);

	if (!CFH_Device.DeviceAlreadyConfigured()) //Wenn das Gerät noch nicht konfiguriert wurde
	{
		Serial.println();
		WiFi.softAP(ssid, password);

		Serial.print("Access Point \"");
		Serial.print(ssid);
		Serial.println("\" started");

		Serial.print("IP address:\t");
		Serial.println(WiFi.softAPIP());

		if (!MDNS.begin(CFH_DNS_Name))
		{
			Serial.println("Error setting up MDNS responder!");
		}
		else
		{
			Serial.println("DNS Started");
			Serial.print("DNS_Name: ");
			Serial.println(CFH_DNS_Name);
		}

		CFHWebServer.onNotFound([]()
			{
				CFHWebServer.send(404, "text/plain", "Page not found");
				Serial.println("Link nicht gefunden");
				Serial.println("---------------------------------------------------------------------------");
			});

		CFHWebServer.on("/ShowEEPROM", []()
			{
				CFHWebServer.send(200, "text/plain", "Showing EEPROM");
				CFH_Device.GetFullEEPROM();
				Serial.println("---------------------------------------------------------------------------");
			});  //GGF: EEPROM auf der Website ausgeben

		CFHWebServer.on("/IsCFH_Device", []()
			{
				CFHWebServer.send(200, "text/plain", "true");
				Serial.println("Checking if AccessPoint is CFH_Device");
				Serial.println("---------------------------------------------------------------------------");
			});

		CFHWebServer.on("/", []()
			{
				CFHWebServer.send(200, "text/plain", "ESP-Startseite!");
				Serial.println("Startseite");
				Serial.println("---------------------------------------------------------------------------");
			});

		CFHWebServer.on("/rest/RegisterDevice", []()
			{
				if (RegisterDevice())
				{
					CFHWebServer.send(200, "text/plain", "Registration succeeded");
					Serial.println("Registration succeeded");
					Serial.println("---------------------------------------------------------------------------");
				}
				else
				{
					CFHWebServer.send(200, "text/plain", "Registration failed");
					Serial.println("Registration failed");
					Serial.println("---------------------------------------------------------------------------");
				}

			});

		CFHWebServer.begin();
	}
	else //Wenn das Gerät bereits konfiguriert wurde
	{
		DeviceConfigured = true;
		Serial.println("CallForHelp Gestartet");
		leds[0] = CRGB(0, 0, 0);
		FastLED.show();
		Serial.println("---------------------------------------------------------------------------");

		Serial.println("Waiting for Input");
	}
}

//loop after setup finished
void loop()
{
	if (DeviceConfigured) //Do Buttonstate if Device is already configured
	{
		int ButtonState = digitalRead(SWITCH_PIN);
		Serial.println(ButtonState);
		if (ButtonState != oldStatus) //Schalter an bei ButtonState == 1 | Schalter aus bei ButtonState == 0
		{
			ButtonSwitched(ButtonState);
		}
		delay(500);
	}
	else
	{
		CFHWebServer.handleClient(); //Do handle if Device is not configured
	}
}

//ButtonSwitched event which detects if the button was falsely switched or not
void ButtonSwitched(int SwitchState)
{
	Serial.print("Button Switched to ");
	Serial.println(SwitchState);
	Serial.println();
	int SwitchTime = 0;
	Serial.print("waiting for confirmation");
	while (digitalRead(SWITCH_PIN) == SwitchState && SwitchTime <= 5)
	{
		Serial.print(".");

		if (SwitchState == 1)
		{
			if (leds[0] == CRGB(0, 0, 0))
			{
				leds[0] = CRGB(255, 0, 0);
			}
			else if (leds[0] == CRGB(255, 0, 0))
			{
				leds[0] = CRGB(0, 0, 0);
			}
		}
		else if (SwitchState == 0)
		{
			if (leds[0] == CRGB(0, 0, 0))
			{
				leds[0] = CRGB(0, 255, 0);
			}
			else if (leds[0] == CRGB(0, 255, 0))
			{
				leds[0] = CRGB(0, 0, 0);
			}
		}
		FastLED.show();
		SwitchTime++;
		delay(1000);
	}
	if (SwitchTime < 5 || digitalRead(SWITCH_PIN) != SwitchState)
	{
		if (SwitchState == 1)
		{
			leds[0] = CRGB(0, 255, 0);
		}
		else if (SwitchState == 0)
		{
			leds[0] = CRGB(255, 0, 0);
		}
		FastLED.show();
		Serial.println("failed");
		delay(2000);
		leds[0] = CRGB(0,0,0);
		FastLED.show();
		return;
	}

	Serial.println("success");
	if (!ConnectToWifi())
	{
		if (!ConnectToMobile())
		{
			Serial.println("No Network connection!");
			return;
		}
	}

	if (SwitchState == 1)
	{
		TriggerAlarm();
	}
	else if (SwitchState == 0)
	{
		DisarmAlarm();
	}
	delay(3000);
	leds[0] = CRGB(0, 0, 0);
	FastLED.show();
}

//Connects the device to wifi
bool ConnectToWifi()
{
	WiFi.begin(NetworkSSID, NetworkPassword);
	int WLANConnectionTime = 0;
	Serial.print("Verbindung zu WLAN wird hergestellt.");

	while (WiFi.status() != WL_CONNECTED && WLANConnectionTime < 15)
	{
		if (leds[0] == CRGB(0, 0, 255))
		{
			leds[0] = CRGB(0, 0, 0);

		}
		else if (leds[0] == CRGB(0, 0, 0))
		{
			leds[0] = CRGB(0, 0, 255);
		}
		FastLED.show();
		delay(500);
		Serial.print(".");
		WLANConnectionTime++;
	}

	//Sofern Zeit kleiner 5sek und Wifi verbunden wird weitergemacht
	if (WLANConnectionTime < 15 && WiFi.status() == WL_CONNECTED)
	{
		Serial.println("Verbunden!");
		Serial.print("IP-Adresse: ");
		Serial.println(WiFi.localIP());
		return true;
	}
	else
	{
		Serial.println("WLAN Verbindung konnte nicht hergestellt werden");
		return false;
	}
}

#pragma endregion

// starting registration of device
bool RegisterDevice()
{
	bool RegistrationSuccess = false;

	Serial.print("Number of Arguments: ");
	Serial.println(CFHWebServer.args());

	//if(paramsNr != (2 || 4)) return;
	if (CFHWebServer.hasArg("userID") && CFHWebServer.hasArg("jwt"))  // Arguments found
	{
		Serial.print("userID: ");
		String UserIDParameter = CFHWebServer.arg("userID");
		Serial.println(UserIDParameter);

		Serial.print("jwt: ");
		String jwtParameter = CFHWebServer.arg("jwt");
		Serial.println(CFHWebServer.arg("jwt"));

		if(ConnectToWifi())
		{
			Serial.println("Testing UserID and JWT");
			CFH_Structs::HTTP_Request_Struct TestUserIDandJWTStruct = CFH_Device.TestUserIDandJWT(UserIDParameter, jwtParameter);
			
			if (TestUserIDandJWTStruct.Success)
			{
				CFH_Device.writeConfigured(jwtParameter, UserIDParameter, TestUserIDandJWTStruct.deviceID);
				RegistrationSuccess = true;
			}
			WiFi.disconnect();
		}
		else
		{
			if (false)   //Use Mobile Connection
			{
				CFH_Structs::HTTP_Request_Struct TestUserIDandJWTStruct = CFH_Device.TestUserIDandJWT(UserIDParameter, jwtParameter);

				if (TestUserIDandJWTStruct.Success)
				{
					CFH_Device.writeConfigured(jwtParameter, UserIDParameter, TestUserIDandJWTStruct.deviceID);

					RegistrationSuccess = true;

					//disconnect Mobile
				}
			}
		}

		if (RegistrationSuccess)
		{
			CFHWebServer.send(200, "text/plain", "true");

			Serial.println("Gerät erfolgreich registriert");
			WiFi.softAPdisconnect();
			CFHWebServer.close();

			Serial.println("---------------------------------------------------------------------------");
		}
		else
		{
			CFHWebServer.send(200, "text/plain", "false");

			Serial.println("Gerät wurde nicht erfolgreich registriert");
			Serial.println("---------------------------------------------------------------------------");
		}

		return RegistrationSuccess;
	}

	else // jwt and UserID not found in arguments
	{
		Serial.println("Necessary arguments not found!");
		Serial.println("All arguments:");
		for (int i = 0; i < CFHWebServer.args(); i++)
		{
			String message = "Arg nº" + (String)i + " –> ";
			message += CFHWebServer.argName(i) + ": ";
			message += CFHWebServer.arg(i) + "\n";
			Serial.println(message);
		}
		CFHWebServer.send(200, "text/plain", "false");

		Serial.println("Gerät wurde nicht erfolgreich registriert");
		Serial.println("---------------------------------------------------------------------------");
	}
}




#pragma region fully_implemented

//Triggers the alarm with specific DeviceID, UserId, JWT, Current or old GPS Position
void TriggerAlarm()
{
	if (CFH_Device.TriggerAlarm(CFH_Device.Connection_Instance.getGPS_position()))
	{
		leds[0] = CRGB(255, 0, 0);
		FastLED.show();
		oldStatus = 1;
	}
	else
	{
		oldStatus = 0;
	}

	Serial.println("---------------------------------------------------------------------------");
}

//Disarms the alarm with specific DeviceID, UserID, JWT
void DisarmAlarm()
{
	if (CFH_Device.DisarmAlarm())
	{
		Serial.println("Alarm disarmed");
		leds[0] = CRGB(0, 255, 0);
		FastLED.show();

		delay(5000);
		leds[0] = CRGB(0, 0, 0);
		FastLED.show();
		oldStatus = 0;
	}
	else
	{
		Serial.println("Alarm not disarmed");
		oldStatus = 1;
	}
	Serial.println("---------------------------------------------------------------------------");
}


#pragma endregion



#pragma region not implemented

bool ConnectToMobile()
{
	return false;
}

#pragma endregion

//GGf. taster anstelle von Schalter unter Abdeckung verwenden da so keine Zustände vorhanden sind