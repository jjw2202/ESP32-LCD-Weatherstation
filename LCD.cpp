#include "Arduino.h"
#include "LCD.h"
#include <Wire.h>

LCDadapt::LCDadapt() {}

LCDadapt::LCDadapt(uint8_t cols, uint8_t rows) {
  this->begin(cols, rows);
}

void LCDadapt::begin(uint8_t cols, uint8_t rows) {
  this->i2caddress = findi2caddress();
  if (i2caddress > 0) this->usesi2c = true;
  if (usesi2c) {
    lcdi2c = LiquidCrystal_I2C(i2caddress, cols, rows);
    lcdi2c.init();
    lcdi2c.backlight();
  } else {
    lcddirect = LiquidCrystal(rs, en, d4, d5, d6, d7);
    lcddirect.begin(cols, rows);
    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(a, PWM_CHANNEL);
    ledcWrite(PWM_CHANNEL, 255);
  }
}

void LCDadapt::print(String text) {
  if (usesi2c) {
    lcdi2c.print(text);
  } else {
    lcddirect.print(text);
  }
}

void LCDadapt::write(uint8_t character) {
  if (usesi2c) {
    lcdi2c.write(character);
  } else {
    lcddirect.write(character);
  }
}

void LCDadapt::clear() {
  if (usesi2c) {
    lcdi2c.clear();
  } else {
    lcddirect.clear();
  }
}

void LCDadapt::setCursor(uint8_t col, uint8_t row) {
  if (usesi2c) {
    lcdi2c.setCursor(col, row);
  } else {
    lcddirect.setCursor(col, row);
  }
}

void LCDadapt::createChar(uint8_t num, unsigned char * data) {
  if (usesi2c) {
    lcdi2c.createChar(num, data);
  } else {
    lcddirect.createChar(num, data);
  }
}

void LCDadapt::setBacklight(uint8_t brightness) {
  if (usesi2c) {
    lcdi2c.setBacklight(brightness);
  } else {
    ledcWrite(PWM_CHANNEL, brightness);
  }
  this->brightness = brightness;
}

void LCDadapt::dim(uint8_t brightness, uint16_t ms) {
  if (usesi2c) return this->setBacklight(brightness);
  if (this->brightness == brightness) return;
  uint32_t startms = millis();
  uint8_t startbrightness = this->brightness;
  while (millis() - startms < ms) {
    double progress = (double)(millis() - startms) / ms;
    this->setBacklight(SigmoidInterpolate(startbrightness, brightness, progress));
    delay(INTERPOLATION_INTERVAL);
  }
}

uint8_t LCDadapt::findi2caddress() {
  //kindly copied from https://randomnerdtutorials.com/esp32-esp8266-i2c-lcd-arduino-ide/
  int8_t error, address, lcdaddress = 0;
  int nDevices = 0;
  //Serial.println("Search I2C address...");
  Wire.begin();
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      //Serial.print("I2C address: 0x");
      if (address<16) {
        //Serial.print("0");
      }
      //Serial.println(address, HEX);
      lcdaddress = address;
      nDevices++;
    }
    else if (error == 4) {
      //Serial.print("Unknow error at address 0x");
      if (address<16) {
        //Serial.print("0");
      }
      //Serial.println(address, HEX);
      lcdaddress = address;
    }    
  }
  if (nDevices == 0) {
    //Serial.println("No I2C devices found");
  }
  return lcdaddress;
}

double LCDadapt::LinearInterpolate(
  double y1, double y2, 
  double mu) {
  return(y1*(1-mu)+y2*mu);
}

double LCDadapt::CosineInterpolate(
  double y1, double y2, 
  double mu) {
  double mu2 = (1-cos(mu*PI))/2;
  return(y1*(1-mu2)+y2*mu2);
}

double LCDadapt::SigmoidInterpolate(
  double y1, double y2,
  double mu) {
  double fa = SIGMOID_FACTOR;
  return (y2-y1)/(1+pow(EULER, (-fa*(mu-0.5))))+y1;
}
