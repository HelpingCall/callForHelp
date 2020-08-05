#include <FastLED.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>

#include <CFH_RegisterDevice.h>


const char* ssid = "CallForHelp_Device"; // The name of the Wi-Fi network that will be created
const char* password = "schnuller";   // The password required to connect to it, leave blank for an open network

#define LED_PIN    5  // Pin für die LED Data
#define NUM_LEDS   3  // Anzahl der LEDs zur Anzeige vom Status etc.
#define SWITCH_PIN D0  // Pin des Schalters

//#define Sim_TX D1 // TX Pin des Sim Moduls
//#define Sim_RX D2 // RX Pin des Sim Moduls

bool DeviceConfigured = false;
int oldStatus = 0;
CRGB leds[NUM_LEDS];

//TODO: DNS Server aktivieren für CallForHelp

ESP8266WebServer CFHWebServer(80);
CFH_RegisterDevice regDevice;

char DeviceConfigurationTemplate[29] = "DeviceAlreadyConfigured:TRUE";

String NetworkSSID = "FRITZ!Box 7530 UM";   // ggf. = CFHWebServer.arg("NetworkSSID");
String NetworkPassword = "Anke1209"; // ggf. = CFHWebServer.arg("NetworkPassword");

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("ESP8266 Started");
  pinMode(SWITCH_PIN, INPUT_PULLDOWN_16);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(20);

  leds[2] = CRGB(0, 0, 255);
  leds[1] = CRGB(0, 0, 255);
  leds[0] = CRGB(0, 0, 255);
  FastLED.show();

  if (!regDevice.DeviceAlreadyConfigured()) //Wenn das Gerät noch nicht konfiguriert wurde
  {
    Serial.println();
    WiFi.softAP(ssid, password);
    
    Serial.print("Access Point \"");
    Serial.print(ssid);
    Serial.println("\" started");

    Serial.print("IP address:\t");
    Serial.println(WiFi.softAPIP());

    if (!MDNS.begin("CallForHelpDevice"))
    {             
      Serial.println("Error setting up MDNS responder!");
    }
    else
    {
      Serial.println("DNS Started");
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
      regDevice.GetFullEEPROM();
      Serial.println("---------------------------------------------------------------------------");
    });

    CFHWebServer.on("/IsCFH_Device", []()
    {
      CFHWebServer.send(200, "text/plain", "true");
      Serial.println("Checking if AccessPoint is CFH_Device");
      Serial.println("---------------------------------------------------------------------------");
    });

    CFHWebServer.on("/writeEEPROM", []()
    {
      CFHWebServer.send(200, "text/plain", "Writing EEPROM");
      EEPROM.begin(786);
      EEPROM.put(30, "abcdefghijklmnopqrstuvwxyz1234567890");
      EEPROM.commit();
      delay(1000);
      Serial.print("Value in EEPROM: ");
      char WritingEEPROM[37];
      EEPROM.get(30, WritingEEPROM);
      Serial.println(WritingEEPROM);
      EEPROM.end();
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
        RegisterDevice();
    });

    CFHWebServer.begin();
  }
  else                           //Wenn das Gerät bereits konfiguriert wurde
  {
    Serial.println("CallForHelp Gestartet");
    leds[0] = CRGB(0, 0, 0);
    leds[1] = CRGB(0, 255, 0);
    leds[2] = CRGB(0, 255, 0);
    FastLED.show();
    Serial.println("---------------------------------------------------------------------------");

    Serial.println("Waiting for Input");
  }
}

void loop()
{
  if(DeviceConfigured) //Do Buttonstate if Device is already configured
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

void RegisterDevice()
{
  bool RegistrationSucceed = false;
  int paramsNr = CFHWebServer.args();
  Serial.print("Number of Arguments: ");
  Serial.println(paramsNr);
  //if(paramsNr != (2 || 4)) return;
  if (CFHWebServer.hasArg("userID") && CFHWebServer.hasArg("jwt"))
  {
    Serial.print("userID: ");
    String UserIDParameter = CFHWebServer.arg("userID");
    Serial.println(UserIDParameter);

    Serial.print("jwt: ");
    String jwtParameter = CFHWebServer.arg("jwt");
    Serial.println(jwtParameter);

    if(ConnectToWifi())
    {
      if(regDevice.TestUserIDandJWT(UserIDParameter, jwtParameter))
      {
        EEPROM.begin(786);

        EEPROM.put(0, DeviceConfigurationTemplate);

        char UID[37];
        UserIDParameter.toCharArray(UID, 37);
        char JWT[513];
        jwtParameter.toCharArray(JWT, 513);


        EEPROM.put(40, UID);

        EEPROM.put(100, JWT);

        Serial.print("EEPROM[0]: ");
        char DCT[29];
        EEPROM.get(0, DCT);
        Serial.println(DCT);

        Serial.print("EEPROM[40]: ");
        char UIDP[37];
        EEPROM.get(40, UIDP);
        Serial.println(UIDP);

        Serial.print("EEPROM[100]: ");
        char JWTP[513];
        EEPROM.get(100, JWTP);
        Serial.println(JWTP);

        EEPROM.commit();
        EEPROM.end();

        RegistrationSucceed = true;
        WiFi.disconnect();
        WiFi.softAPdisconnect();
      }
    }
    else
    {
      if(false)
      {
        if(regDevice.TestUserIDandJWT(UserIDParameter, jwtParameter))
        {
          Serial.print("Putting to EEPROM: ");
          Serial.println(UserIDParameter);
          EEPROM.put(100, UserIDParameter);

          Serial.print("Putting to EEPROM: ");
          Serial.println(jwtParameter);
          EEPROM.put(200, jwtParameter);

          EEPROM.put(30, DeviceConfigurationTemplate);
          EEPROM.commit();
          delay(1000);
          RegistrationSucceed = true;
          //Mobile.disconnect();
        }
      }
    }

    if (RegistrationSucceed)
    {
      CFHWebServer.send(200, "text/plain", "true");

      Serial.println("Gerät erfolgreich registriert");
      CFHWebServer.close();

      Serial.println("---------------------------------------------------------------------------");
    }
    else
    {
      CFHWebServer.send(200, "text/plain", "false");

      Serial.println("Gerät wurde nicht erfolgreich registriert");
      Serial.println("---------------------------------------------------------------------------");
    }
  }
  else
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

    if(SwitchState == 1)
    {
      if(leds[1] == CRGB(0,0,0) && leds[2] == CRGB(0,0,0))
      {
        leds[1] = CRGB(0, 255, 0);
        leds[2] = CRGB(0, 255, 0);
      }
      else if(leds[1] == CRGB(0,255,0) && leds[2] == CRGB(0,255,0))
      {
        leds[1] = CRGB(0, 0, 0);
        leds[2] = CRGB(0, 0, 0);
      }
    }
    else if(SwitchState == 0)
    {
      if(leds[1] == CRGB(0,0,0) && leds[2] == CRGB(0,0,0))
      {
        leds[1] = CRGB(255, 0, 0);
        leds[2] = CRGB(255, 0, 0);
      }
      else if(leds[1] == CRGB(255,0,0) && leds[2] == CRGB(255,0,0))
      {
        leds[1] = CRGB(0, 0, 0);
        leds[2] = CRGB(0, 0, 0);
      }
    }
    FastLED.show();
    SwitchTime++;
    delay(1000);
  }
  if(SwitchTime < 5 && digitalRead(SWITCH_PIN) != SwitchState)
  {
    if(SwitchState == 1)
    {
      leds[1] = CRGB(0, 255, 0);
      leds[2] = CRGB(0, 255, 0);
    }
    else if(SwitchState == 0)
    {
      leds[1] = CRGB(255, 0, 0);
      leds[2] = CRGB(255, 0, 0);
    }
    FastLED.show();
    Serial.println("failed");
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
  else
  {
    Serial.println("failed");
    Serial.println("---------------------------------------------------------------------------");
  }
  leds[0] = CRGB(0, 0, 0);
  FastLED.show();
}

void TriggerAlarm()
{
  EEPROM.begin(786);

  char userID[37];
  EEPROM.get(40, userID);
  Serial.print("ID: ");
  Serial.println(userID);

  char jwt[513];
  EEPROM.get(100, jwt);
  Serial.print("jwt: ");
  Serial.println(jwt);

  EEPROM.end();
  delay(1000);

  String lattitude = String(GetLattitude());
  String longitude = String(GetLongitude());

  String ArmDeviceLink = "http://babyyodahook.xyz/rest/arm?userID=" +String(userID) +"&lat=" + lattitude +"&long=" +longitude +"&jwt=" +String(jwt);

  Serial.print("ArmString: ");
  Serial.println(ArmDeviceLink);

  HTTPClient http;

  http.begin(ArmDeviceLink);
  int httpCode = http.GET();
  Serial.print("Code: ");
  Serial.println(httpCode);
  if (httpCode > 0)
  {
    String httpPayload = http.getString();
    Serial.print("Text: ");
    Serial.println(httpPayload);
    if(httpPayload.equals("true"))
    {
      Serial.println("Alarm triggered");
      leds[1] = CRGB(255, 0, 0);
      leds[2] = CRGB(255, 0, 0);
    }
  }
  else
  {
    Serial.println("Alarm not triggered");
  }
  http.end();

  FastLED.show();
  Serial.println("---------------------------------------------------------------------------");
  oldStatus = 1;
}

void DisarmAlarm()
{
  EEPROM.begin(786);

  char userID[37];
  EEPROM.get(40, userID);
  Serial.print("ID: ");
  Serial.println(userID);

  char jwt[513];
  EEPROM.get(100, jwt);
  Serial.print("jwt: ");
  Serial.println(jwt);

  EEPROM.end();
  delay(1000);

  String DisarmDeviceLink = "http://babyyodahook.xyz/rest/disarm?userID=" +String(userID) +"&jwt=" +String(jwt);

  Serial.print("DisarmString: ");
  Serial.println(DisarmDeviceLink);

  HTTPClient http;

  http.begin(DisarmDeviceLink);
  int httpCode = http.GET();
  Serial.print("Code: ");
  Serial.println(httpCode);
  if (httpCode > 0)
  {
    String httpPayload = http.getString();
    Serial.print("Text: ");
    Serial.println(httpPayload);
    if(httpPayload.equals("true"))
    {
      Serial.println("Alarm disarmed");
      leds[1] = CRGB(0, 255, 0);
      leds[2] = CRGB(0, 255, 0);
      oldStatus = 0;
    }
  }
  else
  {
    Serial.println("Alarm not disarmed");
  }
  http.end();

  FastLED.show();
  Serial.println("---------------------------------------------------------------------------");
}

bool ConnectToWifi()
{
  WiFi.begin(NetworkSSID, NetworkPassword);
  int WLANConnectionTime = 0;
  Serial.print("Verbindung zu WLAN wird hergestellt.");

  while (WiFi.status() != WL_CONNECTED && WLANConnectionTime < 10)
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
  if (WLANConnectionTime < 10 && WiFi.status() == WL_CONNECTED)
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

bool ConnectToMobile()
{
  return false;
}


float GetLattitude()
{
  return 50.0;
}

float GetLongitude()
{
  return 10.0;
}


// Implementiert