#include <SPI.h>
#include <PID_v1.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Adafruit_MAX31855.h>
#include "PID_AutoTune_v0.h"

#define PINS_TEMP_CS           9
#define PINS_SSR               3
#define PINS_LCD_LED           A0
#define PINS_LCD_DC            A1       
#define PINS_LCD_CS            A2
#define PINS_LCD_RST           A3

// Existing global vars
double pid_input, pid_output, pid_setpoint;
int timerSeconds;

// New global vars
boolean tuning = true;
byte reflowOvenTunerModeRemember=2;
double reflowOvenTunerStep=127;
double reflowOvenTunerNoise=10;
double reflowOvenTunerStartValue=127;
unsigned int reflowOvenTunerLookBack=20;
double kp=2;
double ki=0.5;
double kd=2;

// Simulation vars
boolean useSimulation = false;
double kpmodel=1.5, taup=100, theta[50];
double pid_outputStart=5;
unsigned long  modelTime;

// Existing objects
PID reflowOvenPID(&pid_input, &pid_output, &pid_setpoint,kp,ki,kd, DIRECT);
Adafruit_MAX31855 thermocouple(PINS_TEMP_CS);
Adafruit_PCD8544 display = Adafruit_PCD8544(PINS_LCD_DC, PINS_LCD_CS, PINS_LCD_RST);

// New objects
PID_ATune reflowOvenTuner(&pid_input, &pid_output);

// TODO: refactor these vars
unsigned long serialTime;

void setup()
{
  // Initialize LCD
  display.begin();
  display.clearDisplay();
  display.setContrast(30);
  
  pinMode(PINS_LCD_LED, OUTPUT);
  digitalWrite(PINS_LCD_LED, LOW);
  
  timerSeconds = 0;
  pid_setpoint = 150;
  
  if(useSimulation)
  {
    pid_input = 30;
    
    for(byte i=0;i<50;i++)
    {
      theta[i]=pid_outputStart;
    }
    modelTime = 0;
  }
  
  //Setup the pid and tuner
  reflowOvenPID.SetMode(AUTOMATIC);
  reflowOvenTuner.SetControlType(1);    // full PID

  if(tuning)
  {
    tuning=false;
    changeAutoTune();
    tuning=true;
  }
  
  serialTime = 0;
  Serial.begin(9600);

}

void loop()
{
  unsigned long now = millis();

  if(!useSimulation)
  { //pull the pid_input in from the real world
    pid_input = temperature_read();
  }

  if (pid_input > 200) {
    reflowOvenTuner.Cancel();
    display_printAborting(F("overtemp protection"));
    delay(1000);
    
    return;
  }
  
  if(tuning)
  {
    byte val = (reflowOvenTuner.Runtime());
    if (val!=0)
    {
      tuning = false;
    }
    if(!tuning)
    { //we're done, set the tuning parameters
      kp = reflowOvenTuner.GetKp();
      ki = reflowOvenTuner.GetKi();
      kd = reflowOvenTuner.GetKd();
      reflowOvenPID.SetTunings(kp,ki,kd);
      AutoTuneHelper(false);
    }
  }
  else reflowOvenPID.Compute();
  
  if(useSimulation)
  {
    theta[30]=pid_output;
    if(now>=modelTime)
    {
      modelTime +=100; 
      DoModel();
    }
  }
  else
  {
     analogWrite(PINS_SSR,pid_output); 
  }
  
  //send-receive with processing if it's time
  if(millis()>serialTime)
  {
    logger_log(timerSeconds, pid_setpoint, pid_input, pid_output, tuning ? "tuning mode" : "tune complete");
    tuning ? profile_displayStatus() : profile_displayTune();
    timerSeconds++;
    serialTime+=1000;
  }
}

void changeAutoTune()
{
 if(!tuning)
  {
    //Set the pid_output to the desired starting frequency.
    pid_output=reflowOvenTunerStartValue;
    reflowOvenTuner.SetNoiseBand(reflowOvenTunerNoise);
    reflowOvenTuner.SetOutputStep(reflowOvenTunerStep);
    reflowOvenTuner.SetLookbackSec((int)reflowOvenTunerLookBack);
    AutoTuneHelper(true);
    tuning = true;
  }
  else
  { //cancel autotune
    reflowOvenTuner.Cancel();
    tuning = false;
    AutoTuneHelper(false);
  }
}

void AutoTuneHelper(boolean start)
{
  if(start)
    reflowOvenTunerModeRemember = reflowOvenPID.GetMode();
  else
    reflowOvenPID.SetMode(reflowOvenTunerModeRemember);
}

void DoModel()
{
  //cycle the dead time
  for(byte i=0;i<49;i++)
  {
    theta[i] = theta[i+1];
  }
  //compute the pid_input
  pid_input = (kpmodel / taup) *(theta[0]-pid_outputStart) + pid_input*(1-1/taup) + ((float)random(-10,10))/100;

}

double temperature_read(){

  double read = thermocouple.readCelsius();
  
  while (isnan(read)) {
    analogWrite(PINS_SSR, 0);
    Serial.println(F("TC Error!"));
    delay(1000);
    read = thermocouple.readCelsius();
  }

  return read;
}


void logger_log (int timerSeconds, double setpoint, double input, double output, const char* state)
{
  // Send temperature and time stamp to serial
  Serial.print(timerSeconds);
  Serial.print(F("\t"));
  Serial.print(setpoint);
  Serial.print(F("\t"));
  Serial.print(input);
  Serial.print(F("\t"));
  Serial.print(output);
  Serial.print(F("\t"));
  Serial.println(state);
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

void profile_displayStatus(){
    display_printTitle("Tuning Mode");

    display.setCursor(0, 20);
    display.setTextSize(2);
    display.print(pid_input, 1);
    display.setCursor(display.getCursorX(), display.getCursorY() - 6);
    display.write(9);
    display.setCursor(display.getCursorX(), display.getCursorY() + 6);
    display.println(F("C"));
    display.setTextSize(1);
    display.setCursor(0, display.getCursorY() + 4);
    display.print(timerSeconds);
    display.println("s");
    display.display();
}

void profile_displayTune(){
    display_printTitle("Tune Complete");

    display.print(pid_input, 1);
    display.setCursor(display.getCursorX(), display.getCursorY() - 3);
    display.write(9);
    display.setCursor(display.getCursorX(), display.getCursorY() + 3);
    display.print(F("C"));
    display.print(F(" ("));display.print(pid_output / 255 * 100, 0);display.println(F("%)"));
    display.print(F("kp : "));display.println(reflowOvenTuner.GetKp());
    display.print(F("ki : "));display.println(reflowOvenTuner.GetKi());
    display.print(F("kd : "));display.println(reflowOvenTuner.GetKd());
    display.display();
}

// prints aborting message
void display_printAborting(String msg)
{
    display_printTitle(F("Aborting..."));
    display.println(msg);
    display.display();
}
