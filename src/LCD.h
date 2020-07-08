
/*
 * Connection I2C:
 * VDD  to 5V DC
 * GND  to GND (common ground)
 * SCL  to the I²C SCL (at the ESP32 GPIO 22)
 * SDA  to the I²C SDA (at the ESP32 GPIO 21)
 */
#include "globals.h"

/*
 * Connection direct:
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

void lcdsetup();

void lcdwelcometext();
void lcdwelcometext(uint8_t infochar);

String SanitizeText(String text);

#define MAX_ROW_LENGTH 16
#define LCD_SCROLL_START_WAIT 2000 //in ms
#define LCD_SCROLL_SPEED_WAIT 500 //in ms
#define LCD_SCROLL_END_WAIT 1500 //in ms

void lcdprint();
void lcdprint(bool row);
void lcdprint(bool row, String text);
void lcdprint(bool row, String text, uint8_t infochar);
void lcdprint(bool row, String text, uint8_t infochar, bool infocharatend);

void lcdprintloop();

uint32_t calculatescrollmillis(bool row);
uint32_t calculatescrollmillis(bool row, bool fullscroll);
uint32_t calculatescrollmillis(String text);
uint32_t calculatescrollmillis(String text, bool hasinfochar);
uint32_t calculatescrollmillis(String text, bool hasinfochar, bool fullscroll);
