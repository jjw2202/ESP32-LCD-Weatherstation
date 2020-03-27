
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
    genericlcdsetup();
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
    genericlcdsetup();
  }
#endif

void genericlcdsetup() {
}

uint8_t textnum = 255;
void lcdwelcometext() {lcdwelcometext(0);}
void lcdwelcometext(uint8_t infochar) {
  if (textnum == 255) textnum = random(WELCOMETEXT_COUNT);
  String text = welcometext[textnum];
  String rows[] = {"", ""};
  if (text.indexOf(" ") <= 16) {
    uint16_t index = 0, previndex;
    while (index <= 16) {
      previndex = index;
      index = text.indexOf(" ", previndex + 1);
    }
    index = previndex;
    rows[0] = text.substring(0, index);
    rows[0] = String("                ").substring(0, (16 - rows[0].length()) / 2) + rows[0];
    rows[1] = text.substring(index + 1);
    rows[1] = String("                ").substring(0, ((infochar == 0 ? 16 : 15) - rows[1].length()) / 2) + rows[1];
  } else {
    //welcometext should not exceed 16 chars per line
    //will not scroll due to no call to lcdprintloop() in setup
    //maybe can be fixed by starting lcdtask earlier in setup
    rows[0] = text;
  }
  lcdprint(0, rows[0]);
  lcdprint(1, rows[1], infochar, true);
}

String SanitizeText(String text) {
  text.replace("Ä", "ä");
  text.replace("Ö", "ö");
  text.replace("Ü", "ü");
  text.replace("ä", "\341");
  text.replace("ö", "\357");
  text.replace("ü", "\365");
  text.replace("ß", "\342");
  text.replace("°", "\223");
  return text;
}

#define MAX_ROW_LENGTH 16
String rows[] = {"", ""};
uint32_t rowstartms[] = {0, 0};
uint8_t infochars[] = {0, 0};
bool infocharsatend[] = {false, false};
#define LCD_SCROLL_START_WAIT 3000 //in ms
#define LCD_SCROLL_SPEED_WAIT 500 //in ms
#define LCD_SCROLL_END_WAIT 2000 //in ms

void lcdprint() {for (uint8_t row = 0; row < 2; row++) {lcdprint(row, "", 0, false);}}
void lcdprint(bool row) {lcdprint(row, "", 0, false);}
void lcdprint(bool row, String text) {lcdprint(row, text, 0, false);}
void lcdprint(bool row, String text, uint8_t infochar) {lcdprint(row, text, infochar, false);}
void lcdprint(bool row, String text, uint8_t infochar, bool infocharatend) {
  text = SanitizeText(text);
  uint16_t textlength = text.length();
  uint8_t rowlength = (infochar > 0 ? MAX_ROW_LENGTH - 1 : MAX_ROW_LENGTH);
  uint8_t rowoffset = (infochar > 0 && !infocharatend && rowlength < MAX_ROW_LENGTH ? 0 : 1);
  rows[row] = text;
  rowstartms[row] = millis();
  infochars[row] = infochar;
  infocharsatend[row] = infocharatend;
  if (infochar > 0) lcd.createChar(row, chararray[infochar]);
  if (textlength > 16) {
    lcdprintloop();
  } else {
    text.concat(String("                ").substring(0, rowlength - textlength));
    lcd.setCursor(0, row);
    if (infochar > 0 && !infocharatend) lcd.write(byte(row));
    lcd.print(text);
    if (infochar > 0 && infocharatend) lcd.write(byte(row));
  }
}

void lcdprintloop() {
  for (uint8_t row = 0; row < 2; row++) {
    String text = rows[row];
    uint16_t textlength = text.length();
    if (textlength <= 16) continue;
    uint8_t infochar = infochars[row];
    bool infocharatend = infocharsatend[row];
    uint8_t rowlength = (infochar > 0 ? MAX_ROW_LENGTH - 1 : MAX_ROW_LENGTH);
    uint8_t rowoffset = (infochar > 0 && !infocharatend && rowlength < MAX_ROW_LENGTH ? 0 : 1);
    uint32_t startms = rowstartms[row];
    uint32_t now = millis();
    lcd.setCursor(0, row);
    if (infochar > 0 && !infocharatend) lcd.write(row);
    if (now < startms + LCD_SCROLL_START_WAIT) {
      //start wait
      lcd.print(text.substring(0, rowlength));
    } else if (now < startms + LCD_SCROLL_START_WAIT + LCD_SCROLL_SPEED_WAIT * (textlength - rowlength)) {
      //scroll to left
      uint16_t progress = 
        ((double)(now - startms - LCD_SCROLL_START_WAIT) 
        / (LCD_SCROLL_SPEED_WAIT * (textlength - rowlength))) * (textlength - rowlength);
      lcd.print(text.substring(progress, progress + rowlength));
    } else if (now < startms + LCD_SCROLL_START_WAIT + LCD_SCROLL_SPEED_WAIT * (textlength - rowlength) + LCD_SCROLL_END_WAIT) {
      //end wait
      lcd.print(text.substring(textlength - rowlength, textlength));
    } else if (now < startms + LCD_SCROLL_START_WAIT + 2 * LCD_SCROLL_SPEED_WAIT * (textlength - rowlength) + LCD_SCROLL_END_WAIT) {
      //scroll to right
      uint16_t progress = (textlength - rowlength) - 
        ((double)(now - startms - LCD_SCROLL_START_WAIT - LCD_SCROLL_SPEED_WAIT * (textlength - rowlength) - LCD_SCROLL_END_WAIT)
        / (LCD_SCROLL_SPEED_WAIT * (textlength - rowlength))) * (textlength - rowlength);
      lcd.print(text.substring(progress, progress + rowlength));
    } else {
      //reset
      rowstartms[row] = now;
      Serial.println("lcdloop: reached end of scroll animation, resetting!");
      lcdprintloop();
    }
    if (infochar > 0 && infocharatend) lcd.write(row);
  }
}
