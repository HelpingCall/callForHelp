#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <FastLED.h>

ESP8266WebServer server(80);
#define LED_PIN     4
#define NUM_LEDS    3
#define SWITCH_PIN 5

int oldStatus = 0;
CRGB leds[NUM_LEDS];
void setup()
{
    Serial.begin(115200);
    Serial.println("CallForHelp Gestartet");
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(20);


    leds[2] = CRGB(255, 0, 0);
    leds[1] = CRGB(255, 0, 0);
    leds[0] = CRGB(255, 0, 0);
    FastLED.show();

    Serial.println("---------------------------------------------------------------------------");

    server.onNotFound([]()
        {
            server.send(404, "text/plain", "Link wurde nicht gefunden!");
            Serial.println("Link nicht gefunden");
            Serial.println("---------------------------------------------------------------------------");
        });

    server.on("/", []()
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
    if (ButtonState == oldStatus) //Schalter an bei ButtonState == 0 | Schalter aus bei ButtonState == 1
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
    if (!ConnectToWLAN())
    {
        if(!ConnectToMobile())
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
        Serial.println("Alarm not triggered");
    }
    leds[0] = CRGB(255, 0, 0);

    FastLED.show();
    Serial.println("---------------------------------------------------------------------------");
    oldStatus = 1;
}
void DisarmAlarm()
{
    if (ConnectToWLAN())
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

    }
}

bool ConnectToWLAN()
{
    WiFi.begin("UPCFCD4EB1", "sc6ndjbBhs3e");
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
