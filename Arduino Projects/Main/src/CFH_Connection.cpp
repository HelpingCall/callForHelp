#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#include <CFH_Connection.h>

#include <CFH_Structs.h>

//const char* ssid = "CallForHelp_Device"; // The name of the Wi-Fi network that will be created
//const char* password = "schnuller";   // The password required to connect to it, leave blank for an open network

//ESP8266WebServer CFHWebServer(80);
//String NetworkSSID = "FRITZ!Box 7530 UM";   // ggf. = CFHWebServer.arg("NetworkSSID");
//String NetworkPassword = "Anke1209"; // ggf. = CFHWebServer.arg("NetworkPassword");


CFH_Structs::GPS_Position CFH_Connection::getGPS_position()
{
    CFH_Structs::GPS_Position current_gps_position;

    Serial.println("Write AT Commands now");

 	SIM800_send("AT");


 	//Set Error Mode
 	Serial.println(SIM800_send("AT+CMEE=2"));
 	delay(1000);

 	Serial.println(SIM800_send("AT+CPIN=1265"));
 	delay(1000);

 	//Enable Echo if not enabled by default
 	Serial.println(SIM800_send("ATE1"));
 	delay(1000);

 	//Set the SIM800 in GPRS mode
 	Serial.println(SIM800_send("AT+CGATT=1"));
 	delay(1000);

 	//Activate Bearer profile
 	Serial.println(SIM800_send("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\" "));
 	delay(1000);

 	//Set APN options => 'internet.t-mobile'           https://www.hw-group.com/support/international-apn-settings-for-mobile-broadband-network-operators
 	Serial.println(SIM800_send("AT+SAPBR=3,1,\"APN\",\"internet.t-mobile\" "));
 	delay(2000);

 	//Set USER options => 'internet.t-d1.de'
 	Serial.println(SIM800_send("AT+SAPBR=3,1,\"USER\",\"internet.t-d1.de\" "));
 	delay(2000);

 	//Set PWD options => 't-mobile'
 	Serial.println(SIM800_send("AT+SAPBR=3,1,\"PWD\",\"t-mobile\" "));
 	delay(2000);

 	//Open bearer Profile
	 Serial.println(SIM800_send("AT+SAPBR=1,1"));
 	delay(2000);

 	//Get the IP address of the bearer profile
 	Serial.println(SIM800_send("AT+SAPBR=2,1"));
 	delay(2000);

 	//Get GPS Data
 	Serial.println(SIM800_send("AT+CIPGSMLOC=1,1"));
 	delay(2000);

    return current_gps_position;
}


bool CFH_Connection::BooleanHTTPRequest(String RequestLink, String JSON_String)
{
    Serial.print("RequestLink: ");
  	Serial.println(RequestLink);

  	HTTPClient http;
  	http.begin(RequestLink);
	http.addHeader("Content-Type", "application/json");

	int httpCode = http.POST(JSON_String);
	Serial.print("Code: ");
	Serial.println(httpCode);


	if (httpCode > 0)
	{

		if(JSON_Connection_Instance.DeserializeHTTPRequestBoolean(http.getString()))
		{
			Serial.println("success");
			http.end(); //Ends HTTP Request
			return true;
		}
		else
		{
			Serial.println("failed #1");
			http.end(); //Ends HTTP Request
			return false;
		}
	}
	else
	{
		Serial.println("failed #2");
		http.end(); //Ends HTTP Request
		return false;
	}
}

void CFH_Connection::_readSerial() {

	_buffer = "";
	uint64_t timeOld = millis();
	while (!SimStream.available() && !(millis() > timeOld + TIME_OUT_READ_SERIAL)) { ; }
	if(SimStream.available()) { _buffer = SimStream.readString(); }
}

void CFH_Connection::_readSerial(uint32_t timeout) {
	_buffer = "";
	uint64_t timeOld = millis();
	while (!SimStream.available() && !(millis() > timeOld + timeout)) { ; }
	if(SimStream.available()) { _buffer = SimStream.readString(); }
}

String CFH_Connection::SIM800_send(String incoming) //Function to communicate with SIM800 module
{
    SimStream.println(incoming); delay(100); //Print what is being sent to GSM module
    String result = "";
    while (SimStream.available()) //Wait for result
    {
    char letter = SimStream.read();
    result = result + String(letter); //combine char to string to get result
    }
    return result; //return the result
}