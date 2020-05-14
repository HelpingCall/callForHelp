#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <FastLED.h>

ESP8266WebServer server(80);


#define LED_PIN     4
#define NUM_LEDS    3

String page = "";

CRGB leds[NUM_LEDS];

void setup()
{
  //FastLED.setBrightness( 100);
  
  Serial.begin(115200);
  Serial.println("ESP Gestartet");
                                                                                                                                                                                                                                                                 WiFi.begin("FRITZ!Box 7530 UM","Anke1209");   //Freifunkt Lippe/Lemgo
  Serial.print("Verbindung wird hergestellt ...");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Verbunden! IP-Adresse: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  if(WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    http.begin("http://www.babyyodahook.xyz/");
    int httpCode = http.GET();

    if(httpCode > 0)
    {
      String payload = http.getString();
      Serial.print("Text: ");
      Serial.println(payload);
    }
    http.end();
  }

  delay(10000);
}
