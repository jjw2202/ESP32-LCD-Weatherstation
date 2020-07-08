#include "keys.h"
#include <esp_task_wdt.h>

// WiFi credentials need to be defined in wifi.cfg ON SPIFFS!
//   please consult the README for details
// You have to define the OpenWeatherAPI key in keys.h
//   #define OPENWEATHERAPI_KEY "key"
// you can get an OpenWeatherAPI key by registering at home.openweathermap.org/users/sign_up

// CONFIG MOVED TO "globals.h"
#include "globals.h"

uint8_t backlightbrightnessoff = 0; //brightness of lcd when dimmed
uint8_t backlightbrightnesson = 255;  //brightness of lcd when at full brightness

bool backlightstate = true;
uint32_t backlighttimeoutmillis = 0;

extern const char * hostname = "Weatherstation";

extern const uint8_t WELCOMETEXT_COUNT = 14;
extern const String welcometext[] = {
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

#include "interact.h"
#include "LCD.h"
#include "position.h"
#include "spiffshandler.h"
#include "translation.h"
#include "weather.h"
#include "wifihandler.h"

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
