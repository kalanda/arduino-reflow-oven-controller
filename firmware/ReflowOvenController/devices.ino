// Called in interrupt mode when any button is pressed
void keyboard_interrupts()
{
  if ( digitalRead(PINS_BTN_A) && digitalRead(PINS_BTN_B) )
  {
    cancelFlag = false;
  }
  else
  {
    cancelFlag = true;
  }
}

// Scans keyboard in normal mode
void keyboard_scan()
{
   keyboard_scan(false);
}

// Scans keyboard buttons
void keyboard_scan(boolean quickmode)
{
  unsigned long time = millis();

  if (flagHoldKey && !quickmode)
  {
      buzzer_back_tone();
      while( !digitalRead(PINS_BTN_A) || !digitalRead(PINS_BTN_B)) {}
      flagHoldKey = false;
      lastKey = KEY_NONE;

  }
  else if (!digitalRead(PINS_BTN_A))
  {
     while( !digitalRead(PINS_BTN_A) && (millis()-time) <= KEY_HOLD_TIME+KEY_HOLD_TIME_WAIT)
     {
        if (millis()-time >= KEY_DEBOUNCE_TIME) lastKey = KEY_A;
        if (millis()-time >= KEY_HOLD_TIME) { if(!digitalRead(PINS_BTN_B)) lastKey = KEY_ABH; else lastKey = KEY_AH; flagHoldKey = true; }
     }

  }
  else if (!digitalRead(PINS_BTN_B))
  {
     while(!digitalRead(PINS_BTN_B) && (millis()-time) <= KEY_HOLD_TIME+KEY_HOLD_TIME_WAIT)
     {
        if (millis()-time >= KEY_DEBOUNCE_TIME)  lastKey = KEY_B;
        if (millis()-time >= KEY_HOLD_TIME) { if(!digitalRead(PINS_BTN_A)) lastKey = KEY_ABH; else lastKey = KEY_BH; flagHoldKey = true; }
     }
  }
  else
  {
    flagHoldKey = false;
    lastKey = KEY_NONE;
  }
}

// Waits until any key is pressed
void keyboard_waitForAnyKey()
{
   do{ keyboard_scan(); } while (lastKey==KEY_NONE);
}

// Waits until no key is pressed
void keyboard_waitForNokey()
{
   do{ keyboard_scan(); } while (lastKey!=KEY_NONE);
   delay(100);
}

// Plays the back tone on the buzzer
void buzzer_back_tone()
{
  NewTone(PINS_BUZZER, NOTE_FS7, 50);
  delay(100);
  NewTone(PINS_BUZZER, NOTE_C6, 50);
}

// Plays the start tone on the buzzer
void buzzer_start_tone()
{
  NewTone(PINS_BUZZER, NOTE_D6, 100);
  delay(100);
  NewTone(PINS_BUZZER, NOTE_A6, 300);
}

// Plays the finish tone on the buzzer
// Note this method cannot use delay as its called from inside an interrupt
void buzzer_finish_tone()
{
  NewTone(PINS_BUZZER, NOTE_C6, 1000);
}

// Plays the stage change on the buzzer
// Note this method cannot use delay as its called from inside an interrupt
void buzzer_stage_tone()
{
  NewTone(PINS_BUZZER, NOTE_F6, 100);
}

// prints screen title
void display_printTitle(String str)
{
    display.clearDisplay();
    display.drawLine(0, 11, display.width(), 11, BLACK);
    display.setCursor(0,0);
    display.println(str);
    display.println();
    display.display();
}

void display_printEnumeration(byte num, String str, byte currentOption)
{
    display_printEnumerationValue(num, str, String(), currentOption);
}

void display_printEnumerationValue(byte num, String str, String val, byte currentOption)
{
    if(currentOption==num) display.setTextColor(WHITE, BLACK);
    display.print(">");
    display.print(str);
    if(currentOption==num) display.setTextColor(BLACK);
    display.setCursor(display.width() - (val.length() * 6), display.getCursorY());
    display.print(val);
    display.println();
    display.setCursor(display.getCursorX(), display.getCursorY() + 2);
    display.display();
}

// prints aborting message
void display_printAborting()
{
    display.clearDisplay();
    display.println(F("Aborting..."));
    display.display();
}

// prints the temperature monitor and elapsed time
void display_printTemperature(String title, double temperature, int seconds)
{
    // Clear the screen buffer
    display.clearDisplay();

    // Add a title
    display_printTitle(title);

    // Display the temperature
    display.setCursor(0,20);
    display.setTextSize(2);
    display.print(temperature, 1);
    display.setCursor(display.getCursorX(), display.getCursorY() - 6);
    display.write(9);
    display.setCursor(display.getCursorX(), display.getCursorY() + 6);
    display.println(F("C"));
    display.setTextSize(1);

    // Display the elapsed time
    display.setCursor(0, display.getCursorY() + 4);
    display.print(seconds);
    display.println("s");
    display.display();

    // Push the screen buffer out
    display.display();
}

double temperature_read(){

  double read = thermocouple.readCelsius();
  
  while (isnan(read)) {
    analogWrite(PINS_SSR, 0);
    display.clearDisplay();
    display.println(F("TC Error!"));
    display.display();
    delay(1000);
    read = thermocouple.readCelsius();
  }

  return read;
}
