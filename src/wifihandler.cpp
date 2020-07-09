#include "wifihandler.h"
#include <WiFi.h>

void wifisetup() {
  //wifi setup
  wifisettings = loadwifisettings();
  Serial.println("WIFI SSID: " + String(wifisettings.ssid));
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostname);
}

void wificonnect() {
  Serial.print("Connecting to WiFi");
  uint8_t wificounter = 0;
  while(WiFi.status() != WL_CONNECTED && wificounter < WIFI_CONNECT_ATTEMPTS){
    Serial.print(".");
    WiFi.begin(wifisettings.ssid.c_str(), wifisettings.pass.c_str());
    uint8_t delaycounter = 0;
    while(WiFi.status() != WL_CONNECTED && delaycounter++ < 100) delay(100);
    wificounter++;
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi");
  } else {
    Serial.print("WiFi Connection failed: ");
    switch (WiFi.status()) {
      case WL_CONNECTED:
        Serial.print("WL_CONNECTED");
        break;
      case WL_NO_SHIELD:
        Serial.print("WL_NO_SHIELD");
        break;
      case WL_IDLE_STATUS:
        Serial.print("WL_IDLE_STATUS");
        break;
      case WL_NO_SSID_AVAIL:
        Serial.print("WL_NO_SSID_AVAIL");
        break;
      case WL_SCAN_COMPLETED:
        Serial.print("WL_SCAN_COMPLETED");
        break;
      case WL_CONNECT_FAILED:
        Serial.print("WL_CONNECT_FAILED");
        break;
      case WL_CONNECTION_LOST:
        Serial.print("WL_CONNECTION_LOST");
        break;
      case WL_DISCONNECTED:
        Serial.print("WL_DISCONNECTED");
        break;
      default:
        Serial.print("unknown(" + String(WiFi.status()) + ")");
        break;
    }
    Serial.println();
  }
}

void wifiquickconnect() {
  WiFi.begin(wifisettings.ssid.c_str(), wifisettings.pass.c_str());
}
