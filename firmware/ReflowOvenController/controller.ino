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

    if(currentOption==0) display_printEnumeration(0, F("Temp monitor"));
    if(currentOption==1) display_printEnumeration(1, F("Run profile"));

    keyboard_waitForAnyKey();

    if(lastKey==KEY_A) circularList_incrementBy(&currentOption, 0, 1, -1);
    if(lastKey==KEY_B) circularList_incrementBy(&currentOption, 0, 1, 1);
    if(lastKey==KEY_AH) exit = true;
    if(lastKey==KEY_BH)
    {
          if(currentOption==0) runAs_tempMonitor();
          if(currentOption==1) controller_showProfileMenu();
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

    if(currentOption==0) display_printEnumeration(0, F("Leaded"));
    if(currentOption==1) display_printEnumeration(1, F("Lead-free"));

    keyboard_waitForAnyKey();

    if(lastKey==KEY_A) circularList_incrementBy(&currentOption, 0, 1, -1);
    if(lastKey==KEY_B) circularList_incrementBy(&currentOption, 0, 1, 1);
    if(lastKey==KEY_AH) exit = true;
    if(lastKey==KEY_BH)
    {
          if(currentOption==0) runAs_profile(REFLOW_PROFILE_LEADED);
          if(currentOption==1) runAs_profile(REFLOW_PROFILE_LEADFREE);
    }
  }
}