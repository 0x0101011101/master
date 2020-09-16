// Library import
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

// LED, PIEZO, BUTTON, TILT SWITCH params
#define LED_OUTPUT_PIN 2
#define PIEZO_OUTPUT_PIN 3
#define BUTTON_INPUT_PIN 4
#define TILT_SWITCH_INPUT_PIN 5

// NOTE value define
#define NOTE_C7 2100

// ALARM COMMAND params
#define COMMAND_ALARM 0x9

// ALARM TILT SWITCH params
#define ALARM_TYPE_TILT_SWITCH 0x1

// ALARM Value
#define ALARM_OFF 0x0
#define ALARM_ON 0x1

int tiltSwitchValue;
int buttonValue;
int ledBrightness;
int fadeSteps = 5;

boolean alarm = false;

// Initialize an AndroidAccessory object which implements the Open Accessory protocol.
// Most important params are : Manufacturer, Model and Version.
// They will be used in the Android application to verify that you are communicating with the correct ADK board.
// Additionnally, the URI parameter is used by the Android system to look for an appropriate Android application if none is installed.This can be a link to the Android Market or a product page.
// Ex : AndroidAccessory acc("Manufacturer","Model","Description Demo","Version","URI","Serial");
AndroidAccessory acc("0x0101011101","M2MDemoKit12", "M2M DemoKit Arduino Board - Lesson #12", "1.0", "", "0000000001234567");

// Buffer for outgoing data
byte sntmsg[3];

void setup() {

  // Set baud rate for serial communication
  Serial.begin(19200);
  
  // Set Arduino I/O #2 to OUTPUT (LED is connected to)
  pinMode(LED_OUTPUT_PIN, OUTPUT);
   
  // Set Arduino I/O #3 to OUTPUT (PIEZO is connected to)
  pinMode(PIEZO_OUTPUT_PIN, OUTPUT);
    
  // Set Arduino I/O #4 to INPUT (BUTTON is connected to)
  pinMode(BUTTON_INPUT_PIN, INPUT);
    
  // Set Arduino I/O #5 to INPUT (TILT SWITCH is connected to)
  pinMode(TILT_SWITCH_INPUT_PIN, INPUT);

  acc.powerOn();
  
  sntmsg[0] = COMMAND_ALARM;
  sntmsg[1] = ALARM_TYPE_TILT_SWITCH;
}

void loop() {

  // Is something connected ?

  if(acc.isConnected()) {

  // Read current Tilt Switch status
  tiltSwitchValue = digitalRead(TILT_SWITCH_INPUT_PIN);
  
  // Is Tilt Switch value is HIGH (Something move it)
  if((tiltSwitchValue == HIGH) && !alarm) {
    startAlarm();
  }

  //  Alarm ON/OFF Button management
  buttonValue = digitalRead(BUTTON_INPUT_PIN);
  if((buttonValue == LOW) && alarm) {
    stopAlarm();
  }
  
  // If Alarm, fade LED
  if(alarm) {
    fadeLED();
  }
  delay(10);
  }
}


// Start Alarm function
void startAlarm() {
  alarm = true;

  // Play sound on PIEZO
  tone(PIEZO_OUTPUT_PIN, NOTE_C7);
  ledBrightness = 0;
  
  // Inform Android device
  sntmsg[2] = ALARM_ON;
  sendAlarmStateMessage();
}


// Stop Alarm function
void stopAlarm() {
  alarm = false;
  
  // Turn OFF PIEZZO BUZZER
  noTone(PIEZO_OUTPUT_PIN);
  
  // Turn OFF LED
  digitalWrite(LED_OUTPUT_PIN, LOW);
 
  // Inform Android device
  sntmsg[2] = ALARM_OFF;
  sendAlarmStateMessage();
}

// Send ALARM status to Android device
void sendAlarmStateMessage() {
  if (acc.isConnected()) {
    acc.write(sntmsg,3);
  }
}

// Manage LED status
void fadeLED() {
  analogWrite(LED_OUTPUT_PIN, ledBrightness);
  
  // Increase or decrease brightness
  ledBrightness = ledBrightness + fadeSteps;
  
  // Change fade direction when reaching max or min of analog values
  if (ledBrightness < 0 || ledBrightness > 255) {
    fadeSteps = - fadeSteps;
  }
}
