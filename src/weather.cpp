#include "weather.h"
#include <WifiClientSecure.h>
#include <ArduinoJson.h>

uint32_t nextweatherchangems = 0;
uint32_t getnextweatherchangems() {return nextweatherchangems;}
uint8_t screencount = 255;
void weatherloop() {
  if (millis() < nextweatherchangems) return;
  nextweatherchangems = millis() + WEATHER_CHANGE_SCREEN * 1000;

  while(1) {
    uint8_t screencharacter[] = {0, 0};
    String screentext[] = {"", ""};
  
    screencount++;
    if (screencount > 3) screencount = 0;
    switch (screencount) {
      case 0: //city, country | weather description
        screentext[0] = weather.cityname + ", " + weather.countrycode;
        screencharacter[0] = 2;
        screentext[1] = weather.longdescription;
        screencharacter[1] = 0;
        break;
      case 1: //temperature | humidity
        screentext[0] = translation.temperature + ": " + String(weather.temperature, 1) + "°C";
        screencharacter[0] = 4;
        screentext[1] = translation.humidity + ": " + String(weather.humidity, 0) + "%";
        screencharacter[1] = 5;
        break;
      case 2: //wind | clouds
        screentext[0] = translation.wind + ": " + String(weather.wind, 1) + "m/s";
        screencharacter[0] = 6;
        screentext[1] = translation.clouds + ": " + String(weather.cloud, 0) + "%";
        screencharacter[1] = 7;
        break;
      case 3: //rain | snow
        if ((weather.rain != 0) && (weather.snow != 0)) {
          screentext[0] = translation.rain + ": " + String(weather.rain, 1) + "mm/h";
          screencharacter[0] = 8;
          screentext[1] = translation.snow + ": " + String(weather.snow, 1) + "mm/h";
          screencharacter[1] = 9;
        }
        if ((weather.rain != 0) && (weather.snow == 0)) {
          screentext[0] = translation.rain + ": " + String(weather.rain, 1) + "mm/h";
          screencharacter[0] = 8;
          screentext[1] = "";
          screencharacter[1] = 0;
        }
        if ((weather.rain == 0) && (weather.snow != 0)) {
          screentext[0] = translation.snow + ": " + String(weather.snow, 1) + "mm/h";
          screencharacter[0] = 9;
          screentext[1] = "";
          screencharacter[1] = 0;
        }
        if ((weather.rain == 0) && (weather.snow == 0)) continue;
        break;
    }
    for (uint8_t i = 0; i < 2; i++) {
      lcdprint(i, screentext[i], screencharacter[i]);
    }
    break;
  }
  uint32_t scrollmillis = max(calculatescrollmillis(0), calculatescrollmillis(1));
  if (scrollmillis > (WEATHER_CHANGE_SCREEN * 1000)) {
    nextweatherchangems = millis() + scrollmillis;
  }
}

uint32_t nextweatherupdatems = 0;
void checkweatherupdate() {
  if (millis() < nextweatherupdatems) return;
  nextweatherupdatems = millis() + WEATHER_UPDATE_INTERVAL * 1000 * 60;
  updateweather();
}

void updateweather() {
   if (WiFi.status() != WL_CONNECTED) {
    wificonnect();
    if (WiFi.status() != WL_CONNECTED) return;
  }
  Serial.println("Updating weather...");
  if (!position.valid) updateposition();
  weather_t newweather = getcurrentweather(position);
  if (newweather.valid) weather = newweather;
  position.countrycode = weather.countrycode;

  /*
  //dump to serial
  Serial.println("temperature: " + String(weather.temperature));
  Serial.println("humidity: " + String(weather.humidity));
  Serial.println("rain: " + String(weather.rain));
  Serial.println("snow: " + String(weather.snow));
  Serial.println("wind: " + String(weather.wind));
  Serial.println("cloud: " + String(weather.cloud));
  Serial.println("pressure: " + String(weather.pressure));
  Serial.println("feelslike: " + String(weather.feelslike));
  Serial.println("shortdescription: " + String(weather.shortdescription));
  Serial.println("longdescription: " + String(weather.longdescription));
  Serial.println("cityname: " + String(weather.cityname));
  Serial.println("country: " + String(weather.countrycode));
  Serial.println("updatetime: " + String(weather.updatetime));
  */
}

WiFiClientSecure wclient;
weather_t getcurrentweather(pos_t position) {
  // api.openweathermap.org/data/2.5/weather?lat=35&lon=139&units=metric&lang=en
  weather_t weather;
  const char * hostname = "api.openweathermap.org";
  const String url = String("/data/2.5/weather")
    + "?lat=" + String(position.latitude) 
    + "&lon=" + String(position.longitude) 
    + "&units=metric" 
    //+ "&lang=" + (position.countrycode.equalsIgnoreCase("de") ? "de" : "en" ) //if only "de" and "en" should be allowed
    + "&lang=" + position.countrycode  //every countrycode will be allowed
    + "&APPID=" + String(OPENWEATHERAPI_KEY)
  ;
  String answer;
  //Serial.println(String(hostname) + String(url));
  wclient.connect(hostname, 443);
  wclient.print(String("GET ") + url + " HTTP/1.1\r\n"
    + "Host: " + String(hostname) + "\r\n"
    + "Connection: close\r\n\r\n"
    + "Accept: application/json\r\n"
  );
  uint64_t sentmillis = millis();
  while ((millis() - sentmillis <= IP_RESPONSE_TIMEOUT) && !wclient.available()) delay(1);
  if (!wclient.available()) {
    Serial.println("failed to obtain weather data");
    //fatal error in connection
    wclient.stop();
    return weather;
  }
  answer = wclient.readStringUntil('{');
  answer = String("{") + wclient.readStringUntil('\n');
  wclient.stop();
  //Serial.println("Weather data: " + String(answer));
  DynamicJsonDocument data(1024);
  DeserializationError error = deserializeJson(data, answer);
  if (error) {
    Serial.println("error during weather deserialization");
    Serial.println(error.c_str());
    return weather;
  }
  if (!data["cod"].as<String>().equals("200")) {
    //no, "cod" is not a grammar mistake!
    Serial.println("weather api answered with cod: " + data["cod"].as<String>());
    return weather;
  }
  weather.temperature = data["main"]["temp"];
  weather.humidity = data["main"]["humidity"];
  weather.rain = data["rain"]["1h"];
  weather.snow = data["snow"]["1h"];
  weather.wind = data["wind"]["speed"];
  weather.cloud = data["clouds"]["all"];
  weather.pressure = data["main"]["pressure"];
  weather.feelslike = data["main"]["feels_like"];
  weather.shortdescription = data["weather"][0]["main"].as<String>();
  weather.longdescription = data["weather"][0]["description"].as<String>();
  weather.cityname = data["name"].as<String>();
  weather.countrycode = data["sys"]["country"].as<String>();
  weather.updatetime = data["dt"];
  weather.valid = true;
  return weather;
}
