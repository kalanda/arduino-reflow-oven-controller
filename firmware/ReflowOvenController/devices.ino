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
  tone(PINS_BUZZER, 3000);
  delay(time/2);
  tone(PINS_BUZZER, 1000);
  delay(time/2);
  noTone(PINS_BUZZER);
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
