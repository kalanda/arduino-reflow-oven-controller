void runmode_Monitor()
{
   keyboard_waitForNokey();

   // Setup
   unsigned long nextTick = millis();
   timerSeconds = 0;
   lastKey = KEY_NONE;

   // Run the moonitoring loop
   for(boolean exit = false; !exit; )
   {
      if (millis() > nextTick) {
        nextTick += PID_SAMPLE_TIME;
        monitorTick();
      }
    
   		keyboard_scan();
   		if(lastKey==KEY_AH) exit = true;
   }

   // Exit the runmode
   display_printAborting();
   keyboard_waitForNokey();
}

void monitorTick()
{
  // Read the current temperature
  double pid_input = temperature_read();

  // Update the screen
  display_printTemperature(F("Temp monitor"), pid_input, timerSeconds);

  // Log to serial for charts
  logger_log(timerSeconds, 0, pid_input, 0, "monitoring");

  // Increment the tick counter
  timerSeconds++;
}
