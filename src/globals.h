#pragma once
#include <Arduino.h>

#include "keys.h"

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

#define WIFI_CONNECT_ATTEMPTS 3 // max attempts for initiating wifi connection
#define IP_RESPONSE_TIMEOUT 5000

#define LCD_UPDATE_INTERVAL 100 // how often LCD should be refreshed, in ms
#define WDT_TIMEOUT 1000 //WDT timeout in seconds, seems more like milliseconds even though documentation says its in seconds

#ifndef DIMMING_ENABLED
  #undef TOUCH_ENABLED
  #undef SWITCH_ENABLED
#endif

extern uint8_t backlightbrightnessoff;
extern uint8_t backlightbrightnesson;

extern bool backlightstate;
extern uint32_t backlighttimeoutmillis;

extern const char * hostname;

extern const uint8_t WELCOMETEXT_COUNT;
extern const String welcometext[];

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
typedef struct {
  double temperature, humidity, rain, snow, wind, cloud, pressure, feelslike;
  String shortdescription, longdescription, cityname, countrycode;
  bool valid = false;
  uint32_t updatetime;
} weather_t; //weather

extern ia_t ipaddress;
extern pos_t position;
extern ws_t wifisettings;
extern translation_t translation;
extern weather_t weather;

extern void wificonnect();
extern void saveposition();
extern void lcdprint();
extern void lcdprint(bool row);
extern void lcdprint(bool row, String text);
extern void lcdprint(bool row, String text, uint8_t infochar);
extern void lcdprint(bool row, String text, uint8_t infochar, bool infocharatend);
extern uint32_t calculatescrollmillis(bool row);
extern uint32_t calculatescrollmillis(bool row, bool fullscroll);
extern uint32_t calculatescrollmillis(String text);
extern uint32_t calculatescrollmillis(String text, bool hasinfochar);
extern uint32_t calculatescrollmillis(String text, bool hasinfochar, bool fullscroll);
extern void updateposition();
extern ws_t loadwifisettings();