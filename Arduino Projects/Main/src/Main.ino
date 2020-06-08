#include <FastLED.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "FRITZ!Box 7530 UM"; // The name of the Wi-Fi network that will be created
const char* password = "schnuller";   // The password required to connect to it, leave blank for an open network

#define LED_PIN    4  // Pin für die LED Data
#define NUM_LEDS   3  // Anzahl der LEDs zur Anzeige vom Status etc.
#define SWITCH_PIN 5  // Pin des Schalters

#define Sim_TX D1 // TX Pin des Sim Moduls
#define Sim_RX D2 // RX Pin des Sim Moduls

bool DeviceConfigured = false;
int oldStatus = 0;
CRGB leds[NUM_LEDS];

ESP8266WebServer CFHWebServer(80);

String DeviceConfigurationTemplate = "DeviceAlreadyConfigured:TRUE";

String NetworkSSID = "UPCFCD4EB1";   // ggf. = CFHWebServer.arg("NetworkSSID");
String NetworkPassword = "Anke1209"; // ggf. = CFHWebServer.arg("NetworkPassword");

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(1024);

  Serial.println();
  Serial.println("CallForHelp Gestartet");

  Serial.println("---------------------------------------------------------------------------");

  if (!DeviceAlreadyConfigured()) //Wenn das Gerät noch nicht konfiguriert wurde
  {
    Serial.println();
    WiFi.softAP(ssid, password);
    Serial.print("Access Point \"");
    Serial.print(ssid);
    Serial.println("\" started");

    Serial.print("IP address:\t");
    Serial.println(WiFi.softAPIP());

    CFHWebServer.onNotFound([]()
    {
      CFHWebServer.send(404, "text/plain", "Page not found");
      Serial.println("Link nicht gefunden");
      Serial.println("---------------------------------------------------------------------------");
    });

    CFHWebServer.on("/", []()
    {
      CFHWebServer.send(200, "text/plain", "ESP-Startseite!");
      Serial.println("Startseite");
      Serial.println("---------------------------------------------------------------------------");
    });

    CFHWebServer.on("/RegisterDevice", []()
    {
        RegisterDevice();
    });

    CFHWebServer.begin();
  }
  else                           //Wenn das Gerät bereits konfiguriert wurde
  {
    Serial.println("Waiting for Input!");

    pinMode(SWITCH_PIN, INPUT_PULLUP);
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(20);

    leds[2] = CRGB(255, 0, 0);
    leds[1] = CRGB(255, 0, 0);
    leds[0] = CRGB(255, 0, 0);
    FastLED.show();
  }
}

void loop()
{
  if(DeviceConfigured) //Do Buttonstate if Device is already configured
  {
    int ButtonState = digitalRead(SWITCH_PIN);
    if (ButtonState == oldStatus) //Schalter an bei ButtonState == 0 | Schalter aus bei ButtonState == 1
    {
      ButtonSwitched(ButtonState);
    }
    delay(50);
  }
  else
  {
    CFHWebServer.handleClient(); //Do handle if Device is not configured
  }
}

bool DeviceAlreadyConfigured()
{
  if(EEPROM.get(30, DeviceConfigurationTemplate).equals("DeviceAlreadyConfigured:TRUE"))
  {
    Serial.println("EEPROM correct!");
    Serial.print("EEPROM_Value: ");
    Serial.println(EEPROM.get(30, DeviceConfigurationTemplate));
    DeviceConfigured = true;
    return true;
  }
  else
  {
    Serial.println("EEPROM wrong!");
    Serial.print("EEPROM_Value: ");
    Serial.println(EEPROM.get(30, DeviceConfigurationTemplate));
    DeviceConfigured = false;
    return false;
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
    Serial.println(CFHWebServer.arg("userID"));
    String UserIDParameter = CFHWebServer.arg("UserID");

    Serial.print("jwt: ");
    Serial.println(CFHWebServer.arg("jwt"));
    String jwtParameter = CFHWebServer.arg("jwt");

    if(ConnectToWifi(NetworkSSID, NetworkPassword))
    {
      if(TestUserIDandJWT(UserIDParameter, jwtParameter))
      {
        EEPROM.put(100, UserIDParameter);
        EEPROM.put(200, jwtParameter);

        EEPROM.put(30, DeviceConfigurationTemplate);
        RegistrationSucceed = true;
        WiFi.disconnect();
      }
    }
    else
    {
      if(ConnectToMobile())
      {
        if(TestUserIDandJWT(UserIDParameter, jwtParameter))
        {
          EEPROM.put(100, UserIDParameter);
          EEPROM.put(200, jwtParameter);

          EEPROM.put(30, DeviceConfigurationTemplate);
          RegistrationSucceed = true;
          //Mobile.disconnect();
        }
      }
    }
    
    if (RegistrationSucceed)
    {
      CFHWebServer.send(200, "text/plain", "true");

      Serial.println("Gerät erfolgreich registriert");
      Serial.println("---------------------------------------------------------------------------");
      CFHWebServer.close();
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
  }
}

void ButtonSwitched(int SwitchState)
{
  Serial.print("Button Switched to ");
  Serial.println(SwitchState);
  int SwitchTime = 0;
  Serial.print("waiting for confirmation");
  while (digitalRead(SWITCH_PIN) == SwitchState && SwitchTime < 5)
  {
    Serial.print(".");
    SwitchTime++;
    delay(1000);
  }
  if (SwitchTime < 5) return;

  Serial.println("success");

  if (!ConnectToWifi(NetworkSSID, NetworkPassword))
  {
    if (!ConnectToMobile())
    {
      Serial.println("No Network connection!");
      return;
    }
  }
    
  if (SwitchState == 0)
  {
    TriggerAlarm();
  }
  else if (SwitchState == 1)
  {
    DisarmAlarm();
  }
  else
  {
    Serial.println("failed");
    Serial.println("---------------------------------------------------------------------------");
  }
}

void TriggerAlarm()
{
  String userID = GetDataFromEEPROM(100, 36);
  String jwt = GetDataFromEEPROM(200, 512);
  String lattitude = String(GetLattitude());
  String longitude = String(GetLongitude());

  String ArmDeviceLink = "http://babyyodahook.xyz/rest/arm?userID=" +userID +"&lat=" + lattitude +"&long=" +longitude +"&jwt=" +jwt;

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
      leds[1] = CRGB(0, 255, 0);
      leds[2] = CRGB(0, 255, 0);
    }
  }
  else
  {
    Serial.println("Alarm not triggered");
  }
  http.end();
  leds[0] = CRGB(255, 0, 0);

  FastLED.show();
  Serial.println("---------------------------------------------------------------------------");
  oldStatus = 1;
}

void DisarmAlarm()
{
  String userID = GetDataFromEEPROM(100, 36);
  String jwt = GetDataFromEEPROM(200, 512);

  String DisarmDeviceLink = "http://babyyodahook.xyz/rest/disarm?userID=" +userID +"&jwt=" +jwt;

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
      leds[1] = CRGB(255, 0, 0);
      leds[2] = CRGB(255, 0, 0);
    }
  }
  else
  {
    Serial.println("Alarm not disarmed");
  }
  http.end();
  leds[0] = CRGB(255, 0, 0);

  FastLED.show();
  Serial.println("---------------------------------------------------------------------------");
  oldStatus = 0;
}

bool TestUserIDandJWT(String UserID, String JWT)
{
  HTTPClient http;
  String RegisterDeviceCode = "http://babyyodahook.xyz/rest/RegisterDevice?userID=" + String(UserID) + "&jwt=" + String(JWT);
  Serial.print("Trying to register Device: ");
  Serial.println(RegisterDeviceCode);

  http.begin(RegisterDeviceCode);
  int httpCode = http.GET();
  Serial.print("Code: ");
  Serial.println(httpCode);

  if (httpCode > 0)
  {
    String httpPayload = http.getString();
    Serial.print("Text: ");
    Serial.println(httpPayload);
    if (httpPayload.equals("true"))
    {
      http.end();
      return true;
    }
    else
    {
      http.end();
      return false;
    }
  }
  else
  {
    http.end();
    return false;
  }
}

bool ConnectToWifi(String NetworkSSID, String NetworkPassword)
{
  WiFi.begin(NetworkSSID, NetworkPassword);
  int WLANConnectionTime = 0;
  Serial.print("Verbindung zu WLAN wird hergestellt.");

  while (WiFi.status() != WL_CONNECTED && WLANConnectionTime < 10)
  {
    if (leds[0] == CRGB(0, 0, 255))
    {
      leds[0] = CRGB(0, 255, 0);

    }
    else if (leds[0] == CRGB(0, 255, 0))
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

void GetGPS()
{
  
}

void ClearEEPROM()
{
  Serial.println("Deleting");
  delay(2000);
  for (int i = 0 ; i < EEPROM.length() ; i++) 
  {
    EEPROM.write(i, 0);
  }
  delay(1000);

  //Hier die Orte für jwt, userID und DeviceConfigured angeben

  EEPROM.commit();
  Serial.println();
  Serial.println("Finished Deleting EEPROM.old");

  Serial.println();
}

void GetFullEEPROM()
{
  Serial.print("EEPROM: ");
  for (int i = 0 ; i < EEPROM.length() ; i++) 
  {
    Serial.print("[");
    Serial.print(i);
    Serial.print("]");
    Serial.print(char(int(EEPROM.read(i))));
    Serial.print(" : ");
    delay(5);
  }
  Serial.println();
}

String GetDataFromEEPROM(int Address, int DataSize)
{
  Serial.println("Get Data from EEPROM");
  char buffer[DataSize];
  for (int i = 0 ; i < DataSize ; i++) 
  {
    buffer[i] = EEPROM.read(Address + i);
  }
  return String(buffer);
}

float GetLattitude()
{
  return 50.0;
}

float GetLongitude()
{
  return 40.0;
}