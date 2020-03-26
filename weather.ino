
uint32_t nextweatherchangems = 0;
uint8_t screencount = 0;
void weatherloop() {
  if (millis() < nextweatherchangems) return;
  nextweatherchangems = millis() + WEATHER_CHANGE_SCREEN * 1000;

  String screentext[] = {"", ""};

  if (screencount > 3) screencount = 0;
  switch (screencount) {
    case 0: //city, country | weather description
      screentext[0] = weather.cityname + ", " + weather.country;
      lcdprint(0, screentext[0]);
      screentext[1] = weather.longdescription;
      lcdprint(1, screentext[1]);
      screencount++;
      break;
    case 1: //temperature | humidity
      screentext[0] = String("Temp.: ") + String(weather.temperature, 1) + "\337C";
      lcdprint(0, screentext[0]);
      screentext[1] = String("Humidity: ") + String(weather.humidity, 0) + "%";
      lcdprint(1, screentext[1]);
      screencount++;
      break;
    case 2: //wind | clouds
      screentext[0] = String("Wind: ") + String(weather.wind, 1) + "m/s";
      lcdprint(0, screentext[0]);
      screentext[1] = String("Clouds: ") + String(weather.cloud, 0) + "%";
      lcdprint(1, screentext[1]);
      screencount++;
      break;
    case 3: //rain | snow
      screentext[0] = String("Rain: ") + String(weather.rain, 1) + "mm/h";
      lcdprint(0, screentext[0]);
      screentext[1] = String("Snow: ") + String(weather.snow, 1) + "mm/h";
      lcdprint(1, screentext[1]);
      screencount++;
      break;
  }
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
  Serial.println("country: " + String(weather.country));
  Serial.println("updatetime: " + String(weather.updatetime));
}

weather_t getcurrentweather(pos_t position) {
  // api.openweathermap.org/data/2.5/weather?lat=35&lon=139&units=metric&lang=en
  weather_t weather;
  const char * hostname = "api.openweathermap.org";
  const String url = String("/data/2.5/weather")
    + "?lat=" + String(position.latitude) 
    + "&lon=" + String(position.longitude) 
    + "&units=metric" 
    + "&lang=en" 
    + "&APPID=" + String(OPENWEATHERAPI_KEY)
  ;
  String answer;
  //Serial.println(String(hostname) + String(url));
  client.connect(hostname, 443);
  client.print(String("GET ") + url + " HTTP/1.1\r\n"
    + "Host: " + String(hostname) + "\r\n"
    + "Connection: close\r\n\r\n"
    + "Accept: application/json\r\n"
  );
  uint64_t sentmillis = millis();
  while ((millis() - sentmillis <= IP_RESPONSE_TIMEOUT) && !client.available()) delay(1);
  if (!client.available()) {
    Serial.println("failed to obtain weather data");
    //fatal error in connection
    client.stop();
    return weather;
  }
  answer = client.readStringUntil('{');
  answer = String("{") + client.readStringUntil('\n');
  client.stop();
  Serial.println("Weather data: " + String(answer));
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
  weather.country = data["sys"]["country"].as<String>();
  weather.updatetime = data["dt"];
  weather.valid = true;
  return weather;
}
