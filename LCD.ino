
const uint8_t lcd_columns = 16, lcd_rows = 2;
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
    int8_t error, address, lcdaddress;
    int nDevices;
    Serial.println("Search I2C address...");
    for(address = 1; address < 127; address++ ) {
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

  LiquidCrystal_I2C lcd(findlcdaddress(), lcd_columns, lcd_rows);

  void lcdsetup() {
    lcd.init();
    lcd.backlight();
  }
  

void lcdtest() {
  lcd.setCursor(0, 0);
  lcd.print("Hello World");
  lcd.setCursor(0, 1);
  lcd.print("Im the best");
  //lcd.clear();
}
