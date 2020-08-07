#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#include <CFH_Connection.h>

#include <CFH_Structs.h>

const char* ssid = "CallForHelp_Device"; // The name of the Wi-Fi network that will be created
const char* password = "schnuller";   // The password required to connect to it, leave blank for an open network

ESP8266WebServer CFHWebServer(80);
String NetworkSSID = "FRITZ!Box 7530 UM";   // ggf. = CFHWebServer.arg("NetworkSSID");
String NetworkPassword = "Anke1209"; // ggf. = CFHWebServer.arg("NetworkPassword");


CFH_Structs::GPS_Position getGPS_position()
{
    CFH_Structs::GPS_Position current_gps_position;

    current_gps_position.Latitude = 50.00;
    current_gps_position.Longitude = 10.00;

    return current_gps_position;
}