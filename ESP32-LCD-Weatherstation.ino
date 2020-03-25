#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "keys.h"

// WiFi credentials need to be defined in wifi.cfg ON SPIFFS!
//   please consult the README for details
// You have to define the IPAPI key and the OpenWeatherAPI key in keys.h
//   #define IPAPI_KEY "key"
//   #define OPENWEATHERAPI_KEY "key"
// you can get an IPAPI key by contacting them at ipapi.co/contact
// you can get an OpenWeatherAPI key by registering at home.openweathermap.org/users/sign_up


#define WIFI_CONNECT_ATTEMPTS 3 // max attempts for initiating wifi connection
const char * hostname = "Weatherstation";
#define IP_RESPONSE_TIMEOUT 5000

typedef struct {
  double latitude, longitude;
  bool valid = false;
} pos_t; //position
typedef struct {
  String ssid, pass;
} ws_t; //wifisettings
typedef struct {
  String externalip;
  String internalip;
  bool valid = false;
} ia_t; //ipaddress

ia_t ipaddress;
pos_t position;
ws_t wifisettings;

void setup() {
  Serial.begin(115200);

  SPIFFSsetup();
  wifisetup();
  wificonnect();
  updateposition();
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
