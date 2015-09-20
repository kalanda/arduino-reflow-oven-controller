void runAs_tempMonitor()
{
   display_printTitle(F("Temp monitor"));

   keyboard_waitForNokey();
   runAs_tempMonitor_updateTemp();

   // Start timer
   FlexiTimer2::set(1000, runAs_tempMonitor_updateTemp);
   FlexiTimer2::start();

   for(boolean exit = false; !exit; )
   {
   		keyboard_waitForAnyKey();
   		if(lastKey==KEY_AH) exit = true;
   }

   FlexiTimer2::stop();
   keyboard_waitForNokey();
}

void runAs_tempMonitor_updateTemp()
{
  double lastTemperature = temperature_read();

  display_printTitle(F("Temp monitor"));

  lcd.print((int)lastTemperature);
  lcd.write((uint8_t)SYMBOL_DEGREE);
  lcd.print(F("C"));

}