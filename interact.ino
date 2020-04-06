
//keeps track of interactions with user
void interactsetup() {
  #ifdef TOUCH_ENABLED
    //touch setup
    touchsetup();
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
