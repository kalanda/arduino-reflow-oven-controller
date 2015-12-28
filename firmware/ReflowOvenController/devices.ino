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
      buzzer_beep(100);
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

// Beeps buzzer a time in ms
void buzzer_beep(int time)
{
  analogWrite(PINS_BUZZER,64);
  delay(time);
  analogWrite(PINS_BUZZER,0);
}

// prints screen title
void display_printTitle(const __FlashStringHelper* str)
{
    lcd.clear();
    lcd.print(">");
    lcd.print(str);
    lcd.setCursor(0,1);
}

// prints screen title
void display_printTitle(const char* str)
{
    lcd.clear();
    lcd.print(">");
    lcd.print(str);
    lcd.setCursor(0,1);
}

void display_printEnumeration(byte num, const __FlashStringHelper* str)
{
    lcd.print(num, 10);
    lcd.print(".");
    lcd.print(str);
}

// prints aborting message
void display_printAborting()
{
  lcd.clear();
  lcd.print(F("Aborting..."));
}

double temperature_read(){

  double read = thermocouple.readThermocouple(CELSIUS);

  while(
    (read == FAULT_OPEN) ||
    (read == FAULT_SHORT_GND) ||
    (read == FAULT_SHORT_VCC))
  {
    analogWrite(PINS_SSR, 0);
    lcd.clear();
    lcd.print(F("TC Error!"));
    delay(1000);
    read = thermocouple.readThermocouple(CELSIUS);
  }

  return read;
}
