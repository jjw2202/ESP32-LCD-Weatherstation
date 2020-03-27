
#define COUNTRIES_SEPERATOR_LENGTH 3
const String countries = "EN DE ES SP "; // countrycode every 3 chars

void updatetranslation(String countrycode) {
  translation_t t;
  
  uint8_t c = countries.indexOf(countrycode);
  switch (c / COUNTRIES_SEPERATOR_LENGTH) {
    case 1:
      t.temperature = "Temp.";
      t.humidity = "Luftf.";
      t.wind = "Wind";
      t.clouds = "Wolken";
      t.rain = "Regen";
      t.snow = "Schnee";
      break;
    case 2:
    case 3:
      t.temperature = "Temp.";
      t.humidity = "Humedad";
      t.wind = "Viento";
      t.clouds = "Nubes";
      t.rain = "Lluvia";
      t.snow = "Nieve";
    default:
      t.temperature = "Temp.";
      t.humidity = "Humidity";
      t.wind = "Wind";
      t.clouds = "Clouds";
      t.rain = "Rain";
      t.snow = "Snow";
  }
  translation = t;
}