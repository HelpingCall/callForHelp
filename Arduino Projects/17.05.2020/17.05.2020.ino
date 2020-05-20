#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <FastLED.h>

ESP8266WebServer server(80);
#define LED_PIN     4
#define NUM_LEDS    3
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define SWITCH_PIN 5

int oldStatus = 0;
CRGB leds[NUM_LEDS];
void setup()
{
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(20);
  leds[0] = CRGB::Red;
  FastLED.show();
  Serial.begin(115200);
  Serial.println("CallForHelp Gestartet");
  WiFi.begin("UPCFCD4EB1","sc6ndjbBhs3e");

  Serial.print("Verbindung wird hergestellt ...");
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Verbunden!");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());
  leds[2] = CRGB::Red;
  leds[1] = CRGB::Red;
  leds[0] = CRGB::Green;
  FastLED.show();
  
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
  
  server.begin();
}

void loop()
{
  server.handleClient();
  int ButtonState = digitalRead(SWITCH_PIN);
  if(ButtonState == oldStatus) //Schalter an bei ButtonState == 0 | Schalter aus bei ButtonState == 1
  {
    ButtonSwitched(ButtonState);
  }
  delay(100);
}

void ButtonSwitched(int SwitchState)
{
  Serial.print("Button Switched to ");
  Serial.println(SwitchState);
  int SwitchTime = 0;
  Serial.print("waiting for confirmation");
  while(digitalRead(SWITCH_PIN) == SwitchState && SwitchTime<5)
  {
    Serial.print(".");
    SwitchTime++;
    delay(999);
  }
  if(SwitchTime>=5)
  {
    Serial.println("success");
    if(SwitchState == 0)
    {
      TriggerAlarm();
    }
    else if(SwitchState == 1)
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
  leds[1] = CRGB::Green;
  leds[2] = CRGB::Green;
  FastLED.show();
  HTTPClient http;
  http.begin("http://babyyodahook.xyz/triggerAlarm");
  int httpCode = http.GET();
  Serial.print("Code: ");
  Serial.println(httpCode);
  if(httpCode>0)
  {
     String httpPayload = http.getString();
     Serial.print("Text: ");
     Serial.println(httpPayload);
     http.end();
     Serial.println("Alarm triggered");
  }
  else
  {
    Serial.println("Alarm not triggered");
  }
  Serial.println("---------------------------------------------------------------------------");
  oldStatus = 1;
  
}
void DisarmAlarm()
{
  leds[1] = CRGB::Red;
  leds[2] = CRGB::Red;
  FastLED.show();

  HTTPClient http;
  http.begin("http://babyyodahook.xyz/disarmAlarm");
  int httpCode = http.GET();
  Serial.print("Code: ");
  Serial.println(httpCode);
  if(httpCode>0)
  {
     String httpPayload = http.getString();
     Serial.print("Text: ");
     Serial.println(httpPayload);
     http.end();
     Serial.println("Alarm disarmed");
  }
  else
  {
    Serial.println("Alarm not disarmed");
  }
  Serial.println("---------------------------------------------------------------------------");
  oldStatus = 0;
}
