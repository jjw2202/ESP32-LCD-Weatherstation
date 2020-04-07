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

#define WEATHER_CHANGE_SCREEN 5 //in s, floats like 1.5f are allowed

#define WEATHER_UPDATE_INTERVAL 5 //in min
#define POSITION_UPDATE_INTERVAL 2 //in h

//#define DIMMING_ENABLED //uncomment to enable backlight dimming on the lcd

#define TOUCH_ENABLED //uncomment to enable touch based backlight wakeup on the lcd
#define TOUCH_PIN T0 //touch pin for backlight dimming
#define TOUCH_THRESHOLD 80 //greater means more sensitivity
#define TOUCH_MEASURE_TIME 0x4000 //touch measurement timings,
#define TOUCH_CYCLE_TIME 0x8000

#define SWITCH_ENABLED  //uncomment to enable switch based backlight wakeup on the lcd
#define SWITCH_PIN 23 //switch pin for backlight dimming

#define BACKLIGHT_TIMEOUT 30  //in s, timeout for the lcd backlight
#define BACKLIGHT_DIM_TIME 1000  //in ms, how long it takes to dim
uint8_t backlightbrightnessoff = 0; //brightness of lcd when dimmed
uint8_t backlightbrightnesson = 255;  //brightness of lcd when at full brightness

bool backlightstate = true;
uint32_t backlighttimeoutmillis = 0;

#define WIFI_CONNECT_ATTEMPTS 3 // max attempts for initiating wifi connection
const char * hostname = "Weatherstation";
#define IP_RESPONSE_TIMEOUT 5000

#define LCD_UPDATE_INTERVAL 100 // how often LCD should be refreshed, in ms
#define WDT_TIMEOUT 1000 //WDT timeout in seconds, seems more like milliseconds even though documentation says its in seconds

#define WELCOMETEXT_COUNT 14
const String welcometext[] = {
  "Creating the weather",
  "Looking at the weather",
  "Downloading some clouds",
  "Choosing the weather",
  "Looking for clouds",
  "Counting the sun rays",
  "Counting rain drops",
  "Measuring the clouds",
  "Searching for snow",
  "Creating some clouds",
  "Downloading rain drops",
  "Thinking of next season",
  "Listening to the sun",
  "Feeling the temperature",
};

#ifndef DIMMING_ENABLED
  #undef TOUCH_ENABLED
  #undef SWITCH_ENABLED
#endif

typedef struct {
  double latitude, longitude;
  String countrycode;
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
typedef struct {
  String temperature, humidity, wind, clouds, rain, snow;
} translation_t; //translation

ia_t ipaddress;
pos_t position;
ws_t wifisettings;
translation_t translation;

typedef struct {
  double temperature, humidity, rain, snow, wind, cloud, pressure, feelslike;
  String shortdescription, longdescription, cityname, countrycode;
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
  xTaskCreatePinnedToCore(
    lcdtask
    ,  "lcdtask"
    ,  2000  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL 
    ,  1  //  Core
  );
  lcdwelcometext();
  updatetranslation("EN");

  lcdwelcometext(1);
  wifisetup();
  wificonnect();

  loadposition(true);
  if (!position.valid) {
    lcdwelcometext(2);
    updateposition();
  }
  Serial.println("Latitude: " + String(position.latitude));
  Serial.println("Longitude: " + String(position.longitude));
  
  lcdwelcometext(3);
  updateweather();
  updatetranslation(position.countrycode);

  Serial.println("Internal IP Address: " + String(ipaddress.internalip));

  interactsetup();
  
  Serial.println("Starting tasks...");
  
  xTaskCreatePinnedToCore(
    changetask
    ,  "changetask"
    ,  2200  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL 
    ,  1  //  Core
  );
  delay(2000);
  xTaskCreatePinnedToCore(
    updatetask
    ,  "updatetask"
    ,  5500  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL 
    ,  0  //  Core
  );
  delay(5000);
  xTaskCreatePinnedToCore(
    updatepostion
    ,  "updatepostion"
    ,  5500  // Stack size
    ,  NULL
    ,  2  // Priority
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
    vTaskDelay(LCD_UPDATE_INTERVAL);
  }
}

void changetask(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  Serial.println("Started changetask");
  //Serial.println("Task Name: " + String(pcTaskGetTaskName(NULL)));
  Serial.println("esp_task_wdt_add: " + String(esp_err_to_name(esp_task_wdt_add(NULL))));
  //Serial.println("Task on Core: " + String(xPortGetCoreID()));
  //lcdprint(0, "updatetask uxTaskGetStackHighWaterMark: " + String(uxTaskGetStackHighWaterMark(NULL)), 1);  //for testing scrolling lcd text
  //lcdprint(1, "updatetask uxTaskGetStackHighWaterMark: " + String(uxTaskGetStackHighWaterMark(NULL)), 2, true);
  while(1) {
    weatherloop();
    //Serial.println("changetask uxTaskGetStackHighWaterMark: " + String(uxTaskGetStackHighWaterMark(NULL)));
    //vTaskDelay(WEATHER_CHANGE_SCREEN * 1000);
    vTaskDelay(getnextweatherchangems() - millis());
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

void updatepostion(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  Serial.println("Started updatepostion");
  //Serial.println("Task on Core: " + String(xPortGetCoreID()));
  //Serial.println("Task Name: " + String(pcTaskGetTaskName(NULL)));
  //Serial.println("esp_task_wdt_add: " + String(esp_err_to_name(esp_task_wdt_add(NULL))));
  //Serial.println("esp_task_wdt_status: " + String(esp_err_to_name(esp_task_wdt_status(NULL))));
  while(1) {
    updateposition();
    updatetranslation(position.countrycode);
    //Serial.println("updatetask uxTaskGetStackHighWaterMark: " + String(uxTaskGetStackHighWaterMark(NULL)));
    vTaskDelay(POSITION_UPDATE_INTERVAL * 1000 * 60 * 60);
  }
}

void loop() {}  //stub, will never be called, because task is deleted at end of setup
