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

   display_printAborting();
   keyboard_waitForNokey();
}

void runAs_tempMonitor_updateTemp()
{
  double lastTemperature = temperature_read();

  display_printTitle(F("Temp monitor"));

  display.setCursor(0,20);
  display.setTextSize(2);
  display.print(lastTemperature, 1);
  display.setCursor(display.getCursorX(), display.getCursorY() - 6);
  display.write(9);
  display.setCursor(display.getCursorX(), display.getCursorY() + 6);
  display.println(F("C"));
  display.setTextSize(1);
  display.display();

}
