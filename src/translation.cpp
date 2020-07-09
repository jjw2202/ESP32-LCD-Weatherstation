#include "translation.h"

const String countries = "EN DE ES SP FR "; // countrycode every 3 chars

void updatetranslation(String countrycode) {
  translation_t t;
  
  uint8_t c = countries.indexOf(countrycode);
  switch (c / COUNTRIES_SEPERATOR_LENGTH) {
    case 1: //DE
      t.temperature = "Temp.";
      t.humidity = "Luftf.";
      t.wind = "Wind";
      t.clouds = "Wolken";
      t.rain = "Regen";
      t.snow = "Schnee";
      break;
    case 2: //ES
    case 3: //SP
      t.temperature = "Temp.";
      t.humidity = "Humedad";
      t.wind = "Viento";
      t.clouds = "Nubes";
      t.rain = "Lluvia";
      t.snow = "Nieve";
      break;
    case 4: //FR
      t.temperature = "Temp.";
      t.humidity = "Humidite";
      t.wind = "Vent";
      t.clouds = "Nuages";
      t.rain = "Pluie";
      t.snow = "Neige";
      break;
    default:  //EN
      t.temperature = "Temp.";
      t.humidity = "Humidity";
      t.wind = "Wind";
      t.clouds = "Clouds";
      t.rain = "Rain";
      t.snow = "Snow";
  }
  translation = t;
}
