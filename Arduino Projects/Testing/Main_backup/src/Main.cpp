#include <FastLED.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "CallForHelp_171762712"; // The name of the Wi-Fi network that will be created
const char* password = "schnuller";   // The password required to connect to it, leave blank for an open network

#define LED_PIN    4
#define NUM_LEDS   3
#define SWITCH_PIN 5

bool DeviceConfigured = false;
int oldStatus = 0;
CRGB leds[NUM_LEDS];
ESP8266WebServer CFHWebServer(80);


void setup()
{
  Serial.begin(115200);
  EEPROM.begin(1024);


  DeviceAlreadyConfigured();                       //Check if Device is already configured

  Serial.println();
  Serial.println("CallForHelp Gestartet");

  Serial.println("---------------------------------------------------------------------------");

  if (DeviceConfigured != true) //Wenn das Gerät noch nicht konfiguriert wurde
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
  else
  {
    Serial.println("Waiting for Command!");

    pinMode(SWITCH_PIN, INPUT_PULLUP);
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(20);

    leds[2] = CRGB(255, 0, 0);
    leds[1] = CRGB(255, 0, 0);
    leds[0] = CRGB(255, 0, 0);
    FastLED.show();
  }
}

bool DeviceAlreadyConfigured()
{
  if(false)  //EEPROM.get(Address, Type)   -> abfrage ob dort steht, dass das Gerät bereits konfiguriert wurde
  {
    DeviceConfigured = true;
  }
  else
  {
    DeviceConfigured = false;
  }
}

void RegisterDevice()
{
  bool success = false;
  int paramsNr = CFHWebServer.args();
  Serial.print("Number of Arguments: ");
  Serial.println(paramsNr);
  if(paramsNr != (2 || 4)) return;
  if (CFHWebServer.hasArg("userID") && CFHWebServer.hasArg("jwt"))
  {
    Serial.print("userID: ");
    Serial.println(CFHWebServer.arg("userID"));
    Serial.print("jwt: ");
    Serial.println(CFHWebServer.arg("jwt"));
  }
  Serial.println("All Arguments:");
  for (int i = 0; i < CFHWebServer.args(); i++)
  {
    String message = "Arg nº" + (String)i + " –> ";
    message += CFHWebServer.argName(i) + ": ";
    message += CFHWebServer.arg(i) + "\n";
    Serial.println(message);
  }

  String NetworkSSID = "UPCFCD4EB1";   //= CFHWebServer.arg("NetworkSSID");
  String NetworkPassword = "sc6ndjbBhs3e";  //= CFHWebServer.arg("NetworkPassword");

  String UserIDParameter = "07c7eb3e-347b-4878-b81c-f58d6a05cf42&lat=52.024891&long=8.8358521";  //= CFHWebServer.arg("UserID");
  String jwtParameter = "c40adb4ea2e2faa1c81735bf5a333e73674d555f1978f7fadc29ef6359f86bcf62ce04b013daac3d3156826e1df84100b3862a567db419df1dd2571cf934ca237628494d0096496865788d1fdda125e190797ae17f02efaa312eeb877b14e79040355f24cb8e85b0f9bc051a878a9e9a6239a702120071586f01769959c419e088c6256552e6f89ee9419175e5db43c2b1380c54025f64d52f0517c8155fd133aa43689da764dd54b1bcf0820eb80dd1f43e422fd1493a009e07a90760f9ca2281c2b30a0950f0f65cf9fa8c283bf899fbade79c3bfa99cd14acfedcec3efa4520195b704355eac9c1173889dc0ecd3e2df10da18a62dd073b8b62c3cb0ae25c";  //= CFHWebServer.arg("jwt");

  //if(!ConnectToWifi(NetworkSSID, NetworkPassword))
  //{
    //if(testUserIDandJWT(UserIDParameter, jwtParameter))
    //{
        //Save to EEPROM
    //}
  //}
  //else
  //{
    //userid and jwt not saved
  //}
  if (success)
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
    delay(999);
  }
  if (SwitchTime >= 5)
  {
    Serial.println("success");
    if (SwitchState == 0)
    {
      TriggerAlarm();
    }
    else if (SwitchState == 1)
    {
      DisarmAlarm();
    }
  }
  else
  {
    Serial.println("failed");
    Serial.println("---------------------------------------------------------------------------");
  }
}

void TriggerAlarm()
{
  if (!ConnectToWifi("UPCFCD4EB1", "sc6ndjbBhs3e"))
  {
    if (!ConnectToMobile())
    {
      return;
    }
  }
  HTTPClient http;
  http.begin("http://babyyodahook.xyz/triggerAlarm");
  int httpCode = http.GET();
  Serial.print("Code: ");
  Serial.println(httpCode);
  if (httpCode > 0)
  {
    String httpPayload = http.getString();
    Serial.print("Text: ");
    Serial.println(httpPayload);
    http.end();
    Serial.println("Alarm triggered");
    leds[1] = CRGB(0, 255, 0);
    leds[2] = CRGB(0, 255, 0);
  }
  else
  {
    Serial.println("Couldn't arm the Alarm");
  }
  leds[0] = CRGB(255, 0, 0);

  FastLED.show();
  Serial.println("---------------------------------------------------------------------------");
  oldStatus = 1;
}

void DisarmAlarm()
{
  if (ConnectToWifi("UPCFCD4EB1", "sc6ndjbBhs3e"))
  {
    HTTPClient http;
    http.begin("http://babyyodahook.xyz/disarmAlarm");
    int httpCode = http.GET();
    Serial.print("Code: ");
    Serial.println(httpCode);
    if (httpCode > 0)
    {
      String httpPayload = http.getString();
      Serial.print("Text: ");
      Serial.println(httpPayload);
      http.end();
      Serial.println("Alarm disarmed");
      leds[1] = CRGB(255, 0, 0);
      leds[2] = CRGB(255, 0, 0);
    }
    else
    {
      Serial.println("Alarm not disarmed");
    }
    leds[0] = CRGB(255, 0, 0);
    FastLED.show();
    Serial.println("---------------------------------------------------------------------------");
    oldStatus = 0;
  }
  else
  {
    Serial.println("Couldn't disarm the Alarm");
  }
}


bool testUserIDandJWT(String UserID, String JWT)
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
    if (httpPayload == "true")
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
  Serial.print("EEPROM.old: ");
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