#include "LCD.h"
#include "LCDadapt.h"
#include "customlcdcharacters.h"

const uint8_t lcd_columns = 16, lcd_rows = 2;

LCDadapt lcd = LCDadapt();

void lcdsetup() {
  lcd.begin(lcd_columns, lcd_rows);
  // nothing to do here, its all initialized in LCDadapt class
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
  text.replace("°", "\337");
  return text;
}

String rows[] = {"", ""};
uint32_t rowstartms[] = {0, 0};
uint8_t infochars[] = {0, 0};
bool infocharsatend[] = {false, false};

void lcdprint() {for (uint8_t row = 0; row < 2; row++) {lcdprint(row, "", 0, false);}}
void lcdprint(bool row) {lcdprint(row, "", 0, false);}
void lcdprint(bool row, String text) {lcdprint(row, text, 0, false);}
void lcdprint(bool row, String text, uint8_t infochar) {lcdprint(row, text, infochar, false);}
void lcdprint(bool row, String text, uint8_t infochar, bool infocharatend) {
  text = SanitizeText(text);
  rows[row] = text;
  rowstartms[row] = millis();
  infochars[row] = infochar;
  infocharsatend[row] = infocharatend;
}

void lcdprintloop() {
  for (uint8_t row = 0; row < 2; row++) {
    String text = rows[row];
    uint16_t textlength = text.length();
    uint8_t infochar = infochars[row];
    if (infochar > 0) lcd.createChar(row, chararray[infochar]);
    bool infocharatend = infocharsatend[row];
    uint8_t rowlength = (infochar > 0 ? MAX_ROW_LENGTH - 1 : MAX_ROW_LENGTH);
    //uint8_t rowoffset = (infochar > 0 && !infocharatend && rowlength < MAX_ROW_LENGTH ? 0 : 1);
    uint32_t startms = rowstartms[row];
    uint32_t now = millis();
    lcd.setCursor(0, row);
    if (infochar > 0 && !infocharatend) lcd.write(row);
    if (textlength > rowlength) {
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
        //Serial.println("lcdprintloop: reached end of scroll animation, resetting!");
        lcdprintloop();
      }
    } else {
      text.concat(String("                ").substring(0, rowlength - textlength));
      lcd.print(text);
    }
    if (infochar > 0 && infocharatend) lcd.write(row);
  }

  //Backlight
  #ifdef DIMMING_ENABLED
    if (millis() - backlighttimeoutmillis > BACKLIGHT_TIMEOUT * 1000) backlightstate = false;
    if (backlightstate) {
      lcd.dim(backlightbrightnesson, BACKLIGHT_DIM_TIME);
    } else {
      lcd.dim(backlightbrightnessoff, BACKLIGHT_DIM_TIME);
    }
  #endif
}

uint32_t calculatescrollmillis(bool row) {return calculatescrollmillis(row, false);}
uint32_t calculatescrollmillis(bool row, bool fullscroll) {return calculatescrollmillis(rows[row], infochars[row], fullscroll);}
uint32_t calculatescrollmillis(String text) {return calculatescrollmillis(text, false);}
uint32_t calculatescrollmillis(String text, bool hasinfochar) {return calculatescrollmillis(text, hasinfochar, false);}
uint32_t calculatescrollmillis(String text, bool hasinfochar, bool fullscroll) {
  uint8_t rowlength = (hasinfochar ? 15 : 16);
  uint16_t textlength = text.length();
  if (textlength <= rowlength) return false;
  if (fullscroll) return 2 * LCD_SCROLL_START_WAIT + 2 * LCD_SCROLL_SPEED_WAIT * (textlength - rowlength) + LCD_SCROLL_END_WAIT;
  return LCD_SCROLL_START_WAIT + LCD_SCROLL_SPEED_WAIT * (textlength - rowlength) + LCD_SCROLL_END_WAIT;
}
