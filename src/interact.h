#include "globals.h"

void interactsetup();

#ifdef TOUCH_ENABLED
  void touchsetup();
  void touchISR();
#endif

#ifdef SWITCH_ENABLED
  void switchsetup();
  void switchISR();
#endif
