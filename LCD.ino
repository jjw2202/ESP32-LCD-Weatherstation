
const uint8_t lcd_columns = 16, lcd_rows = 2;

#ifdef LCDI2C
  #include <LiquidCrystal_I2C.h>

  /*
   * Connection:
   * VDD  to 5V DC
   * GND  to GND (common ground)
   * SCL  to the I²C SCL (at the ESP32 GPIO 22)
   * SDA  to the I²C SDA (at the ESP32 GPIO 21)
   */

  int8_t findlcdaddress() {
  //kindly copied from https://randomnerdtutorials.com/esp32-esp8266-i2c-lcd-arduino-ide/
    int8_t error, address, lcdaddress = 0;
    int nDevices = 0;
    Serial.println("Search I2C address...");
    Wire.begin();
    for(address = 1; address < 127; address++) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      if (error == 0) {
        Serial.print("I2C address: 0x");
        if (address<16) {
          Serial.print("0");
        }
        Serial.println(address, HEX);
        lcdaddress = address;
        nDevices++;
      }
      else if (error == 4) {
        Serial.print("Unknow error at address 0x");
        if (address<16) {
          Serial.print("0");
        }
        Serial.println(address, HEX);
        lcdaddress = address;
      }    
    }
    if (nDevices == 0) {
      Serial.println("No I2C devices found");
    }
    return lcdaddress;
  }

  LiquidCrystal_I2C lcd(0,0,0);

  void lcdsetup() {
    lcd = LiquidCrystal_I2C(findlcdaddress(), lcd_columns, lcd_rows);
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, lcdwificonnected);
    lcd.createChar(1, lcdpositionupdated);
    lcd.createChar(2, lcdweatherupdated);
  }
  
#else
  #include <LiquidCrystal.h>

  /*
   * Connection:
   * VSS  to GND
   * VDD  to 5V DC
   * V0   to contrast via potentiometer to 5V DC and GND
   * RS   to GPIO 13
   * RW   to GND
   * E    to GPIO 12
   * D0   to -
   * D1   to -
   * D2   to -
   * D3   to -
   * D4   to GPIO 14
   * D5   to GPIO 27
   * D6   to GPIO 33
   * D7   to GPIO 32
   * A    to via 220Ω resistor at 5V DC
   * K    to GND
   */

  const uint8_t rs = 13, en = 12, d4 = 14, d5 = 27, d6 = 33, d7 = 32;
  LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

  void lcdsetup() {
    lcd.begin(lcd_columns, lcd_rows);
    lcd.createChar(0, lcdwificonnected);
    lcd.createChar(1, lcdpositionupdated);
    lcd.createChar(2, lcdweatherupdated);
  }
#endif

void lcdstart() {
  lcd.setCursor(2, 0);
  lcd.print("Looks at the");
  lcd.setCursor(3, 1);
  lcd.print("weather...");
}

void lcdstatus1() {
  lcd.setCursor(15, 1);
  lcd.write(0);
}

void lcdstatus2() {
  lcd.setCursor(15, 1);
  lcd.write(1);
}

void lcdstatus3() {
  lcd.setCursor(15, 1);
  lcd.write(2);
}

void lcdtest() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hello World");
  lcd.setCursor(0, 1);
  lcd.print("Im the best");
  //lcd.clear();
}
