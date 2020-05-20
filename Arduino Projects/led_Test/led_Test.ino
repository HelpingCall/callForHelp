#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FastLED.h>

ESP8266WebServer server(80);


#define LED_PIN     4
#define NUM_LEDS    4

unsigned long delayTime;
String page = "";

CRGB leds[NUM_LEDS];

void setup()
{
  Serial.begin(115200);
  Serial.println("ESP Gestartet");
  WiFi.begin("FRITZ!Box 7530 UM","Anke1209");

  Serial.print("Verbindung wird hergestellt ...");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Verbunden! IP-Adresse: ");
  Serial.println(WiFi.localIP());

  if(MDNS.begin("ESP8266_1"))
  {
    Serial.println("DNS gestartet!");
  }

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

  Serial.println("---------------------------------------------------------------------------");
  
  server.onNotFound([]()
  {
    server.send(404, "text/plain", "Link wurde nicht gefunden!");
    Serial.println("Link nicht gefunden");
    Serial.println("---------------------------------------------------------------------------");
  });

  server.on("/" , []()
  {
    server.send(200, "text/plain", "ESP-Startseite!");
    Serial.println("Startseite");
    Serial.println("---------------------------------------------------------------------------");
  });

  server.on("/rot" , []()
  {
    leds[0] = CRGB(255, 0, 0);
    leds[1] = CRGB(255, 0, 0);
    leds[2] = CRGB(255, 0, 0);
    leds[3] = CRGB(255, 0, 0);
    FastLED.show();
    delay(500);
    Serial.println("---------------------------------------------------------------------------");
  });

   server.on("/gruen" , []()
  {
    leds[0] = CRGB(0, 255, 0);
    leds[1] = CRGB(0, 255, 0);
    leds[2] = CRGB(0, 255, 0);
    leds[3] = CRGB(0, 255, 0);
    FastLED.show();
    delay(500);
    Serial.println("---------------------------------------------------------------------------");
  });

   server.on("/blau" , []()
  {
    leds[0] = CRGB(0, 0, 255);
    leds[1] = CRGB(0, 0, 255);
    leds[2] = CRGB(0, 0, 255);
    leds[3] = CRGB(0, 0, 255);
    FastLED.show();
    delay(500);
    Serial.println("---------------------------------------------------------------------------");
  });

  
  server.begin();
}

void loop()
{
  server.handleClient();
}
