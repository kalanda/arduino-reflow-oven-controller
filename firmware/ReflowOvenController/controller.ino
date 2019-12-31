// Runs the controller
void controller_run()
{
   controller_showMainMenu();
}

// Main Menu View
void controller_showMainMenu()
{
  byte currentOption = 0;

  for(boolean exit = false;!exit;)
  {
    display_printTitle(F("Main Menu"));

    display_printEnumeration(0, F("Temp monitor"), currentOption);
    display_printEnumeration(1, F("Run profile"), currentOption);
    display_printEnumeration(2, F("Settings"), currentOption);

    keyboard_waitForAnyKey();

    if(lastKey==KEY_A) circularList_incrementBy(&currentOption, 0, 2, 1);
    if(lastKey==KEY_AH) exit = true;
    if(lastKey==KEY_B)
    {
          if(currentOption==0) runmode_Monitor();
          if(currentOption==1) controller_showProfileMenu();
          if(currentOption==2) controller_showSettingsMenu();
    }
  }
}

// Main Menu View
void controller_showProfileMenu()
{
  byte currentOption = 0;

  for(boolean exit = false;!exit;)
  {
    display_printTitle(F("Run profile"));

    display_printEnumeration(0, F("Leaded"), currentOption);
    display_printEnumeration(1, F("Lead-free"), currentOption);

    keyboard_waitForAnyKey();

    if(lastKey==KEY_A) circularList_incrementBy(&currentOption, 0, 1, 1);
    if(lastKey==KEY_AH) exit = true;
    if(lastKey==KEY_B)
    {
          if(currentOption==0) runmode_Profile(REFLOW_PROFILE_LEADED);
          if(currentOption==1) runmode_Profile(REFLOW_PROFILE_LEADFREE);
    }
  }
}

// Settings Menu View
void controller_showSettingsMenu()
{
  byte currentOption = 0;

  for(boolean exit = false;!exit;)
  {
    bool currentBacklight = digitalRead(PINS_LCD_LED);
    uint8_t currentContrast = display.getContrast();

    String backlight = "OFF";
    if(currentBacklight==LOW) backlight = "ON";
    
    display_printTitle(F("Settings"));

    display_printEnumerationValue(0, F("Backlight"), backlight, currentOption);
    display_printEnumerationValue(1, F("Contrast"), String(currentContrast / 10), currentOption);
    display_printEnumerationValue(2, F("Simulate"), simulation ? F("ON") : F("OFF"), currentOption);

    keyboard_waitForAnyKey();

    if(lastKey==KEY_A) circularList_incrementBy(&currentOption, 0, 2, 1);
    if(lastKey==KEY_AH) exit = true;
    if(lastKey==KEY_B)
    {
          if(currentOption==0) toggle_backlight();
          if(currentOption==1) increase_contrast();
          if(currentOption==2) simulation = !simulation;
    }
  }
}
