
// increment the value of a variable using minimun and maximun limits (byte version)
void circularList_incrementBy(byte *value, byte minimun, byte maximun, int incrementBy) {

  if (incrementBy>0) {

    if (*value+incrementBy > maximun) *value = minimun;
    else *value = *value+incrementBy;

  }
  else {

    if ( (*value < minimun) || (*value < (incrementBy*(-1)) )) *value = maximun;
    else *value = *value+incrementBy;

  }

}

// toggle the backlight and store the value in EEPROM for reboots
void toggle_backlight() {
  bool currentBacklight = digitalRead(PINS_LCD_LED);
  
  digitalWrite(PINS_LCD_LED, !currentBacklight);
  EEPROM.update(ADDR_BACKLIGHT, !currentBacklight);
}


// increase the contrast (looping around if necessary) and store the value in EEPROM for reboots
void increase_contrast() {
  uint8_t currentContrast = display.getContrast();
  
  circularList_incrementBy(&currentContrast, 10, 100, 10);
  display.setContrast(currentContrast);
  EEPROM.update(ADDR_CONTRAST, currentContrast);
}
