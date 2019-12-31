// Run profile
void runmode_Profile(int profile)
{
   keyboard_waitForNokey();

   // Define the reflow profile
   if (profile == REFLOW_PROFILE_LEADED) {

      currentReflowProfile[0].name = "Pre-heat";
      currentReflowProfile[0].targetTemperature = 150;
      currentReflowProfile[0].durationInSeconds = 1;
      currentReflowProfile[0].elapsedTime = 0;
      currentReflowProfile[0].pid_kp = PID_KP_PREHEAT;
      currentReflowProfile[0].pid_ki = PID_KI_PREHEAT;
      currentReflowProfile[0].pid_kd = PID_KD_PREHEAT;

      currentReflowProfile[1].name = "Soak";
      currentReflowProfile[1].targetTemperature = 180;
      currentReflowProfile[1].durationInSeconds = 90;
      currentReflowProfile[1].elapsedTime = 0;
      currentReflowProfile[1].pid_kp = PID_KP_SOAK;
      currentReflowProfile[1].pid_ki = PID_KI_SOAK;
      currentReflowProfile[1].pid_kd = PID_KD_SOAK;

      currentReflowProfile[2].name = "Ramp";
      currentReflowProfile[2].targetTemperature = 220;
      currentReflowProfile[2].durationInSeconds = 1;
      currentReflowProfile[2].elapsedTime = 0;
      currentReflowProfile[2].pid_kp = PID_KP_PREHEAT;
      currentReflowProfile[2].pid_ki = PID_KI_PREHEAT;
      currentReflowProfile[2].pid_kd = PID_KD_PREHEAT;

      currentReflowProfile[3].name = "Reflow";
      currentReflowProfile[3].targetTemperature = 220;
      currentReflowProfile[3].durationInSeconds = 30;
      currentReflowProfile[3].elapsedTime = 0;
      currentReflowProfile[3].pid_kp = PID_KP_REFLOW;
      currentReflowProfile[3].pid_ki = PID_KP_REFLOW;
      currentReflowProfile[4].pid_kd = PID_KP_REFLOW;

      currentReflowProfile[4].name = "Cool";
      currentReflowProfile[4].targetTemperature = 45;
      currentReflowProfile[4].durationInSeconds = 1;
      currentReflowProfile[4].elapsedTime = 0;
      currentReflowProfile[4].pid_kp = PID_KP_PREHEAT;
      currentReflowProfile[4].pid_ki = PID_KP_PREHEAT;
      currentReflowProfile[4].pid_kd = PID_KP_PREHEAT;
   }

   if (profile == REFLOW_PROFILE_LEADFREE) {

      currentReflowProfile[0].name = "Pre-heat";
      currentReflowProfile[0].targetTemperature = 150;
      currentReflowProfile[0].durationInSeconds = 1;
      currentReflowProfile[0].elapsedTime = 0;
      currentReflowProfile[0].pid_kp = PID_KP_PREHEAT;
      currentReflowProfile[0].pid_ki = PID_KI_PREHEAT;
      currentReflowProfile[0].pid_kd = PID_KD_PREHEAT;

      currentReflowProfile[1].name = "Soak";
      currentReflowProfile[1].targetTemperature = 220;
      currentReflowProfile[1].durationInSeconds = 90;
      currentReflowProfile[1].elapsedTime = 0;
      currentReflowProfile[1].pid_kp = PID_KP_SOAK;
      currentReflowProfile[1].pid_ki = PID_KI_SOAK;
      currentReflowProfile[1].pid_kd = PID_KD_SOAK;

      currentReflowProfile[2].name = "Ramp";
      currentReflowProfile[2].targetTemperature = 225;
      currentReflowProfile[2].durationInSeconds = 1;
      currentReflowProfile[2].elapsedTime = 0;
      currentReflowProfile[2].pid_kp = PID_KP_PREHEAT;
      currentReflowProfile[2].pid_ki = PID_KI_PREHEAT;
      currentReflowProfile[2].pid_kd = PID_KD_PREHEAT;

      currentReflowProfile[3].name = "Reflow";
      currentReflowProfile[3].targetTemperature = 255;
      currentReflowProfile[3].durationInSeconds = 60;
      currentReflowProfile[3].elapsedTime = 0;
      currentReflowProfile[3].pid_kp = PID_KP_REFLOW;
      currentReflowProfile[3].pid_ki = PID_KP_REFLOW;
      currentReflowProfile[3].pid_kd = PID_KP_REFLOW;

      currentReflowProfile[4].name = "Cool";
      currentReflowProfile[4].targetTemperature = 45;
      currentReflowProfile[4].durationInSeconds = 1;
      currentReflowProfile[4].elapsedTime = 0;
      currentReflowProfile[4].pid_kp = PID_KP_PREHEAT;
      currentReflowProfile[4].pid_ki = PID_KP_PREHEAT;
      currentReflowProfile[4].pid_kd = PID_KP_PREHEAT;
   }
   
   // Setup runmode
   unsigned long nextTick = millis();
   timerSeconds = 0;
   currentStage = 0;
   startTemp = temperature_read();
   lastKey = KEY_NONE;

   buzzer_start_tone();

   // Run the moonitoring loop
   for(boolean exit = false; !exit; )
   {
      if (millis() > nextTick) { 
        nextTick += PID_SAMPLE_TIME; 
        profileTick(); 
      }
    
      keyboard_scan();
      if(lastKey==KEY_AH) exit = true;
   }

   // Turn off the heater
   analogWrite(PINS_SSR, 0);

   // Exit the runmode
   display_printAborting();
   keyboard_waitForNokey();

}

void profileTick()
{
  // Shut down and short circuit if we've finished
  if (currentStage > REFLOW_STAGE_COOL) { analogWrite(PINS_SSR, 0); display_printTitle(F("Complete!")); return; }
  
  // Read the current temperature
  pid_input = temperature_read();

  // Update the stage
  if ( stageIsComplete() ) advanceToNextStage();
  
  // Run PID computation
  pid_setPoint = profile_calculateSetPoint();
  reflowOvenPID.Compute();
  
  // Put PID output to SSR
  analogWrite(PINS_SSR, pid_output);
  
  // Update the screen
  display_printTemperature(currentReflowProfile[currentStage].name, pid_input, timerSeconds);
  
  // Log to serial for charts
  logger_log(timerSeconds, pid_setPoint, pid_input, pid_output, currentReflowProfile[currentStage].name);
  
  // Increment our timers
  currentReflowProfile[currentStage].elapsedTime++;
  timerSeconds++;
}

// Check if the current stage has been completed
boolean stageIsComplete() {
  boolean tempReached = pid_input >= currentReflowProfile[currentStage].targetTemperature;
  boolean durationReached = currentReflowProfile[currentStage].elapsedTime >= currentReflowProfile[currentStage].durationInSeconds;

  // Cooling stage gets treated diffrent, we need the temp to be UNDER the setpoint
  if (currentStage == REFLOW_STAGE_COOL) {
    tempReached = pid_input <= currentReflowProfile[currentStage].targetTemperature;
  }

  return tempReached & durationReached;
}

void advanceToNextStage() {
  // Notiy the user audibly
  buzzer_stage_tone();
  
  // Increment the stage counter
  currentStage++;

  // Set PID tunings for the new stage
  reflowOvenPID.SetTunings(currentReflowProfile[currentStage].pid_kp, currentReflowProfile[currentStage].pid_ki, currentReflowProfile[currentStage].pid_kd);
}

double profile_calculateSetPoint(){

   // Set start temperature for this stage
   double startTemperature = 0;
   if (currentStage == REFLOW_STAGE_PREHEAT) {
      startTemperature = startTemp;
   } else {
      startTemperature = currentReflowProfile[currentStage-1].targetTemperature;
   }

   // Set end temperature for this stage
   double endTemperature = currentReflowProfile[currentStage].targetTemperature;
   int duration = currentReflowProfile[currentStage].durationInSeconds;

   // Set elapsed time for this stage
   int elapsedTime = 0;
   if(currentReflowProfile[currentStage].elapsedTime >= duration) {
      elapsedTime = duration;
   } else {
      elapsedTime = currentReflowProfile[currentStage].elapsedTime+1;
   }

   // returns the calculated set point
   return startTemperature+(((endTemperature-startTemperature)/duration)*elapsedTime);
}
