#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FastLED.h>

ESP8266WebServer server(80);


#define LED_PIN     4
#define NUM_LEDS    3
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

unsigned long delayTime;
String page = "";

CRGB leds[NUM_LEDS];

int Hue = 0;
void setup()
{
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness( 100);
  
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
  leds[2] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[0] = CRGB::Green;
  FastLED.show();
  Serial.println(WiFi.localIP());

  if(MDNS.begin("ESP8266-LED"))
  {
    Serial.println("DNS gestartet!");
  }

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

  server.on("/aus" , []()
  {
    server.send(200, "text/plain", "led strip power off");
    leds[1] = CRGB::Black;
    leds[2] = CRGB::Black;
    FastLED.show();
    delay(500);
    Serial.println("aus");
    Serial.println("---------------------------------------------------------------------------");
  });

   server.on("/white" , []()
  {
    server.send(200, "text/plain", "led strip color set to white!");
    leds[1] = CRGB::White;
    leds[2] = CRGB::White;
    FastLED.show();
    delay(500);
    Serial.println("weiß");
    Serial.println("---------------------------------------------------------------------------");
  });

  server.on("/red" , []()
  {
    server.send(200, "text/plain", "led strip color set to red");
    leds[1] = CRGB::Red;
    leds[2] = CRGB::Red;
    FastLED.show();
    delay(500);
    Serial.println("rot");
    Serial.println("---------------------------------------------------------------------------");
  });

   server.on("/green" , []()
  {
    server.send(200, "text/plain", "led strip color set to green!");
    leds[1] = CRGB::Green;
    leds[2] = CRGB::Green;
    FastLED.show();
    delay(500);
    Serial.println("grün");
    Serial.println("---------------------------------------------------------------------------");
  });

   server.on("/blue" , []()
  {
    server.send(200, "text/plain", "led strip color set to blue!");
    leds[1] = CRGB::Blue;
    leds[2] = CRGB::Blue;
    FastLED.show();
    delay(500);
    Serial.println("blau");
    Serial.println("---------------------------------------------------------------------------");
  });
  
  server.begin();
}

void loop()
{
  server.handleClient();
}
