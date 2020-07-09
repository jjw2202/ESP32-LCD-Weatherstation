
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>

#define PWM_FREQUENCY 5000
#define PWM_CHANNEL 0
#define PWM_RESOLUTION 8 //8-bit resolution
const uint8_t rs = 13, en = 12, d4 = 14, d5 = 27, d6 = 33, d7 = 32, a = 25;
#define SIGMOID_FACTOR 30
#define INTERPOLATION_INTERVAL 10 //in ms, how often a

class LCDadapt {
  public:
    LCDadapt();
    LCDadapt(uint8_t cols, uint8_t rows);
    void begin(uint8_t cols, uint8_t rows);
    void print(String text);
    void write(uint8_t character);
    void clear(void);
    void setCursor(uint8_t col, uint8_t row);
    void createChar(uint8_t num, unsigned char * data);
    void setBacklight(uint8_t brightness);
    void dim(uint8_t brightness, uint16_t ms);
  private:
    uint8_t findi2caddress(void);
    uint8_t i2caddress = 0;
    bool usesi2c = false;
    uint8_t brightness = 0;
    LiquidCrystal_I2C lcdi2c = LiquidCrystal_I2C(0,0,0);
    LiquidCrystal lcddirect = LiquidCrystal(rs, en, d4, d5, d6, d7);
    double LinearInterpolate(double y1, double y2, double mu);
    double CosineInterpolate(double y1, double y2, double mu);
    double SigmoidInterpolate(double y1, double y2, double mu);
};
