#include <WiFi.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
// WiFi credentials need to be defined in wifi.cfg ON SPIFFS!
//   please consult the README for details
#define WIFI_CONNECT_ATTEMPTS 3 // max attempts for initiating wifi connection
const char * hostname = "Weatherstation";
ws_t wifisettings;
void setup() {
  Serial.begin(115200);

  SPIFFSsetup();
  wifisetup();
  wificonnect();
}

void loop() {
  // put your main code here, to run repeatedly:

}
