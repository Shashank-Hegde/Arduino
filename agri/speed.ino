#if !defined(ESP32)
#error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
#elif ( defined(ARDUINO_ESP32S3_DEV) || defined(ARDUINO_ESP32_S3_BOX) || defined(ARDUINO_TINYS3) || \
        defined(ARDUINO_PROS3) || defined(ARDUINO_FEATHERS3) )
#error ESP32_S3 is not supported yet
#endif

// These define's must be placed at the beginning before #include "TimerInterrupt_Generic.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
#define _TIMERINTERRUPT_LOGLEVEL_     0
#define dirPin 26
#define stepPin 27
#define enPin 25

#define stepsPerRevolution 800

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "ESP32TimerInterrupt.h"

// Don't use PIN_D1 in core v2.0.0 and v2.0.1. Check https://github.com/espressif/arduino-esp32/issues/5868
#define PIN_D2              2         // Pin D2 mapped to pin GPIO2/ADC12/TOUCH2/LED_BUILTIN of ESP32
#define PIN_D3              3         // Pin D3 mapped to pin GPIO3/RX0 of ESP32
#define PIN_D14              14         // Pin D4 mapped to pin GPIO4/ADC10/TOUCH0 of ESP32

unsigned int SWPin = PIN_D14;
int motor_duration = 2000;

#define TIMER0_INTERVAL_MS        1
#define DEBOUNCING_INTERVAL_MS    80

#define LOCAL_DEBUG               1

// Init ESP32 timer 0
ESP32Timer ITimer0(0);

volatile unsigned long rotationTime = 0;

// Not using float => using RPM = 100 * real RPM
float RPM       = 0;
float avgRPM    = 0;
//uint32_t RPM       = 0;
//uint32_t avgRPM    = 0;

volatile int debounceCounter;

// With core v2.0.0+, you can't use Serial.print/println in ISR or crash.
// and you can't use float calculation inside ISR
// Only OK in core v1.0.6-
bool IRAM_ATTR TimerHandler0(void * timerNo)
{
  if ( !digitalRead(SWPin) && (debounceCounter >= DEBOUNCING_INTERVAL_MS / TIMER0_INTERVAL_MS ) )
  {
    //min time between pulses has passed
    // Using float calculation / vars in core v2.0.0 and core v2.0.1 will cause crash
    // Not using float => using RPM = 100 * real RPM
    RPM = ( 6000000 / ( rotationTime * TIMER0_INTERVAL_MS ) );

    avgRPM = ( 2 * avgRPM + RPM) / 3;
    
    if (avgRPM>700)
    {
      motor_duration = 500;
    }
    else
    {
      motor_duration = 2000;
    }

    rotationTime = 0;
    debounceCounter = 0;
  }
  else
  {
    debounceCounter++;
  }

  //if (rotationTime >= 5000)
  if (rotationTime >= 1000)
  {
    // If idle, set RPM to 0, don't increase rotationTime
    RPM = 0;

    avgRPM = ( avgRPM + 3 * RPM) / 4;

    rotationTime = 0;
  }
  else
  {
    rotationTime++;
  }

  return true;
}

void setup()
{
  pinMode(SWPin, INPUT_PULLUP);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enPin,OUTPUT);
  digitalWrite(enPin,LOW);

  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting RPM_Measure on ")); Serial.println(ARDUINO_BOARD);
  Serial.println(ESP32_TIMER_INTERRUPT_VERSION);
  Serial.print(F("CPU Frequency = ")); Serial.print(F_CPU / 1000000); Serial.println(F(" MHz"));

  // Using ESP32  => 80 / 160 / 240MHz CPU clock ,
  // For 64-bit timer counter
  // For 16-bit timer prescaler up to 1024

  // Interval in microsecs
  if (ITimer0.attachInterruptInterval(TIMER0_INTERVAL_MS * 1000, TimerHandler0))
  {
    Serial.print(F("Starting  ITimer0 OK, millis() = ")); Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer0. Select another freq. or timer"));

  Serial.flush();
}

void loop()
{
  
  
  if (avgRPM >= 650)
  {
    Serial.print(F("RPM  = ")); Serial.print((float) RPM / 100.f); Serial.print(F(", avgRPM  = ")); Serial.println((float) avgRPM / 100.f);
    // Set the spinning direction clockwise:
  digitalWrite(dirPin, LOW);

  // Spin the stepper motor 1 revolution slowly:
  for (int i = 0; i < stepsPerRevolution; i++) {
   // These four lines result in 1 step:
   digitalWrite(stepPin, HIGH);
   delayMicroseconds(motor_duration);
   digitalWrite(stepPin, LOW);
   delayMicroseconds(motor_duration);
  }
  }
  
  if (avgRPM < 650)
  {
    Serial.print(F("RPM  = ")); Serial.print((float) RPM / 100.f); Serial.print(F(", avgRPM  = ")); Serial.println((float) avgRPM / 100.f);
     digitalWrite(dirPin, HIGH);

  // Spin the stepper motor 1 revolution slowly:
  for (int i = 0; i < stepsPerRevolution; i++) {
   // These four lines result in 1 step:
   digitalWrite(stepPin, HIGH);
   delayMicroseconds(motor_duration);
   digitalWrite(stepPin, LOW);
   delayMicroseconds(motor_duration);
  }

  delay(1000);
  }
  
}
