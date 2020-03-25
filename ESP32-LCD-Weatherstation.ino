
#define LCDI2C    //define this line if you use I2C for your LCD

void setup() {

  Serial.begin(115200);
  lcdsetup();

}

void loop() {

  lcdtest();

}
