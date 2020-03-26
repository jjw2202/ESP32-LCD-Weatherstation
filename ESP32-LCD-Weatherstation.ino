#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "keys.h"
#include "customlcdcharacters.h"
#include <esp_task_wdt.h>

// WiFi credentials need to be defined in wifi.cfg ON SPIFFS!
//   please consult the README for details
// You have to define the IPAPI key and the OpenWeatherAPI key in keys.h
//   #define IPAPI_KEY "key"
//   #define OPENWEATHERAPI_KEY "key"
// you can get an IPAPI key by contacting them at ipapi.co/contact
// you can get an OpenWeatherAPI key by registering at home.openweathermap.org/users/sign_up

#define LCDI2C //uncomment if youre using I2C to control the LCD

#define WEATHER_CHANGE_SCREEN 5 //in s, floats like 1.5f are allowed

#define WEATHER_UPDATE_INTERVAL 2 //in min

#define WIFI_CONNECT_ATTEMPTS 3 // max attempts for initiating wifi connection
const char * hostname = "Weatherstation";
#define IP_RESPONSE_TIMEOUT 5000

#define WDT_TIMEOUT 1000 //WDT timeout in seconds, seems more like milliseconds even though documentation says its in seconds

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

typedef struct {
  double temperature, humidity, rain, snow, wind, cloud, pressure, feelslike;
  String shortdescription, longdescription, cityname, country;
  bool valid = false;
  uint32_t updatetime;
} weather_t;

weather_t weather;

void setup() {
  Serial.begin(115200);

  Serial.println("FreeRTOS info:");
  Serial.println("esp_task_wdt_init: " + String(esp_err_to_name(esp_task_wdt_init(WDT_TIMEOUT, false))));
  Serial.println("Setup running on Core: " + String(xPortGetCoreID()));
  Serial.println("Setup Task Name: " + String(pcTaskGetTaskName(NULL)));
  
  SPIFFSsetup();
  lcdsetup();
  lcdstart();

  lcdstatus1();
  wifisetup();
  wificonnect();
  lcdstatus2();
  updateposition();
  
  lcdstatus3();
  updateweather();
  
  Serial.println("Starting tasks...");
  
  xTaskCreatePinnedToCore(
    lcdtask
    ,  "lcdtask"
    ,  2000  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL 
    ,  0  //  Core
  );
  delay(1000);
  xTaskCreatePinnedToCore(
    changetask
    ,  "changetask"
    ,  2200  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL 
    ,  0  //  Core
  );
  delay(5000);
  xTaskCreatePinnedToCore(
    updatetask
    ,  "updatetask"
    ,  5500  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL 
    ,  1  //  Core
  );
  Serial.println("Setup finished");
  vTaskDelete(NULL); //we dont need this task anymore
}


void lcdtask(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  Serial.println("Started lcdtask");
  //Serial.println("Task Name: " + String(pcTaskGetTaskName(NULL)));
  Serial.println("esp_task_wdt_add: " + String(esp_err_to_name(esp_task_wdt_add(NULL))));
  //Serial.println("Task on Core: " + String(xPortGetCoreID()));
  while(1) {
    lcdprintloop();
    //Serial.println("lcdprint uxTaskGetStackHighWaterMark: " + String(uxTaskGetStackHighWaterMark(NULL)));
    vTaskDelay(500);
  }
}

void changetask(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  Serial.println("Started changetask");
  //Serial.println("Task Name: " + String(pcTaskGetTaskName(NULL)));
  Serial.println("esp_task_wdt_add: " + String(esp_err_to_name(esp_task_wdt_add(NULL))));
  //Serial.println("Task on Core: " + String(xPortGetCoreID()));
  while(1) {
    weatherloop();
    //Serial.println("changetask uxTaskGetStackHighWaterMark: " + String(uxTaskGetStackHighWaterMark(NULL)));
    vTaskDelay(WEATHER_CHANGE_SCREEN * 1000);
  }
}

void updatetask(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  Serial.println("Started updatetask");
  //Serial.println("Task on Core: " + String(xPortGetCoreID()));
  //Serial.println("Task Name: " + String(pcTaskGetTaskName(NULL)));
  Serial.println("esp_task_wdt_add: " + String(esp_err_to_name(esp_task_wdt_add(NULL))));
  //Serial.println("esp_task_wdt_status: " + String(esp_err_to_name(esp_task_wdt_status(NULL))));
  while(1) {
    updateweather();
    //Serial.println("updatetask uxTaskGetStackHighWaterMark: " + String(uxTaskGetStackHighWaterMark(NULL)));
    vTaskDelay(WEATHER_UPDATE_INTERVAL * 1000 * 60);
  }
}

void loop() {}  //stub, will never be called, because task is deleted at end of setup
