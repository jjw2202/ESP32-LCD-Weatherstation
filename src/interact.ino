
//keeps track of interactions with user
void interactsetup() {
  #ifdef TOUCH_ENABLED
    //touch setup
    touchsetup();
  #endif
  #ifdef SWITCH_ENABLED
    //switch setup
    switchsetup();
  #endif
}

//touch
#ifdef TOUCH_ENABLED
  void touchsetup() {
    backlighttimeoutmillis = millis();
    touchSetCycles(TOUCH_MEASURE_TIME, TOUCH_CYCLE_TIME);
    touchAttachInterrupt(TOUCH_PIN, touchISR, TOUCH_THRESHOLD);
  }
  
  void touchISR() {
    Serial.println("touch detected: " + String(touchRead(TOUCH_PIN)));
    backlightstate = true;
    backlighttimeoutmillis = millis();
  }
#endif

//switch (pin)
#ifdef SWITCH_ENABLED
  void switchsetup() {
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    backlighttimeoutmillis = millis();
    // https://techtutorialsx.com/2017/09/30/esp32-arduino-external-interrupts/
    attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), switchISR, FALLING);
  }
  
  void switchISR() {
    Serial.println("switch detected");
    backlightstate = true;
    backlighttimeoutmillis = millis();
  }
#endif
