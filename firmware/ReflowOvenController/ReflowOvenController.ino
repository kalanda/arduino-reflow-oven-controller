// Import libraries
#include <SPI.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Adafruit_MAX31855.h>
#include <PID_v1.h>
#include <NewTone.h>
#include "pitches.h"

// Version
#define CODE_MAJOR_VERSION     2  // major version
#define CODE_MINOR_VERSION     0  // minor version

// Pin settings
#define PINS_TEMP_CS           9
#define PINS_SSR               3
#define PINS_BUZZER            7
#define PINS_BTN_A             6
#define PINS_BTN_B             8
#define PINS_LCD_LED           A0
#define PINS_LCD_DC            A1       
#define PINS_LCD_CS            A2
#define PINS_LCD_RST           A3

// Keycodes
#define KEY_NONE             0 // No keys pressed
#define KEY_A                1 // Button A was pressed
#define KEY_B                2 // Button B was pressed
#define KEY_AH               3 // Button A was pressed and holded (KEY_HOLD_TIME) milisecons
#define KEY_BH               4 // Button B was pressed and holded (KEY_HOLD_TIME) milisecons
#define KEY_ABH              5 // Buttons A+B was pressed and holded (KEY_HOLD_TIME) milisecons

// EEPROM Addresses
#define ADDR_BACKLIGHT          1
#define ADDR_CONTRAST           2

// Keyboard times
#define KEY_DEBOUNCE_TIME    30 // debounce time (ms) to prevent flickering when pressing or releasing the button
#define KEY_HOLD_TIME       400 // holding period (ms) how long to wait for press+hold event
#define KEY_HOLD_TIME_WAIT  100 // Used for double key holding

// PID values
#define PID_SAMPLE_TIME     1000     // Milliseconds
#define PID_KP_PREHEAT         5.00
#define PID_KI_PREHEAT         0.01
#define PID_KD_PREHEAT         0.00
#define PID_KP_SOAK            5.00
#define PID_KI_SOAK            0.01
#define PID_KD_SOAK            0.00
#define PID_KP_REFLOW          5.00
#define PID_KI_REFLOW          0.01
#define PID_KD_REFLOW          0.00

#define REFLOW_PROFILE_LEADED      0
#define REFLOW_PROFILE_LEADFREE    1

#define REFLOW_STAGE_PREHEAT       0
#define REFLOW_STAGE_SOAK          1
#define REFLOW_STAGE_RAMP          2
#define REFLOW_STAGE_REFLOW        3
#define REFLOW_STAGE_COOL          4

// Struct to hold the reflow stage parameters.
typedef struct ReflowStage {
  const char* name;
  double targetTemperature;
  int durationInSeconds;
  int elapsedTime;
  double pid_kp;
  double pid_ki;
  double pid_kd;
} ReflowStage_t;

// Array to hold the individual reflow stages together as a complete reflow profile
ReflowStage currentReflowProfile[5];

// PID variables
double pid_setPoint;
double pid_input;
double pid_output;
double pid_kp = PID_KP_PREHEAT;
double pid_ki = PID_KI_PREHEAT;
double pid_kd = PID_KD_PREHEAT;

// Specify PID control interface
PID reflowOvenPID(&pid_input, &pid_output, &pid_setPoint, pid_kp, pid_ki, pid_kd, DIRECT);

// Thermocouple object instance
Adafruit_MAX31855 thermocouple(PINS_TEMP_CS);

// Display control object instance
Adafruit_PCD8544 display = Adafruit_PCD8544(PINS_LCD_DC, PINS_LCD_CS, PINS_LCD_RST);

// Variables used on interrupt mode
volatile boolean cancelFlag = false;    // Flag used to abort interrupt mode

// Global variables
uint8_t lastKey = KEY_NONE;     // Last key pressed
boolean flagHoldKey = false;    // Flag to ignore keys after a hold key
int currentStage = 0;
int timerSeconds = 0;
double startTemp;

#define SIMULATION_FLOOR       25

boolean simulation = false;
double simulationVelocity = 0;
double simulatedTemp = SIMULATION_FLOOR;

// Setup before start
void setup()
{
  // Initialize EEPROM data
  uint8_t contrast = EEPROM.read(ADDR_CONTRAST);
  contrast = ((contrast >= 10) && (contrast <= 100)) ? contrast : 50;
  
  // Initialize LCD
  display.begin();
  display.clearDisplay();
  display.setContrast(contrast);
  
  // Pinmode inputs
  pinMode(PINS_BTN_A, INPUT_PULLUP);
  pinMode(PINS_BTN_B, INPUT_PULLUP);

  // Pinmode outputs
  pinMode(PINS_LCD_LED,     OUTPUT);
  pinMode(PINS_BUZZER,      OUTPUT);

  // Restore the backlight setting
  digitalWrite(PINS_LCD_LED, EEPROM.read(ADDR_BACKLIGHT));

  // Tell the PID to range between 0 and 255 (PWM Output)
  reflowOvenPID.SetOutputLimits(0, 255);

  // Turn the PID on
  reflowOvenPID.SetMode(AUTOMATIC);
  reflowOvenPID.SetSampleTime(PID_SAMPLE_TIME);

  // Put SSR to 0 duty
  analogWrite(PINS_SSR, 0);

  // Init serial
  Serial.begin(9600);
}

// Main loop
void loop()
{
  controller_run();
}
