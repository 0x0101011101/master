// Library import
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

// LED, PIEZO, BUTTON, IR LIGHT BARRIER input PIN
#define BUTTON_INPUT_PIN 2
#define PIEZO_OUTPUT_PIN 3
#define LED_ALARM_ON_PIN 4
#define LED_ALARM_OFF_PIN 5
#define IR_LIGHT_BARRIER_INPUT_PIN A0

// IR LIGHT THRESHOLD value >> WARNING : Depend on IR Receiver / Emitter LED models params (cf datasheet) - Receiver & Emitter must have same frequencies
#define IR_LIGHT_BARRIER_THRESHOLD 260

// NOTE value define
#define NOTE_C7 2100

// ALARM COMMAND params
#define COMMAND_ALARM 0x9

// ALARM TYPE IR LIGHT BARRIER COMMAND value
#define ALARM_TYPE_IR_LIGHT_BARRIER 0x2

// ALARM ON/OFF Value
#define ALARM_OFF 0x0
#define ALARM_ON 0x1

int irLightBarrierValue;   
int buttonValue;
int ledBrightness = 0;

boolean alarm = false;

// Initialize an AndroidAccessory object which implements the Open Accessory protocol.
// Most important params are : Manufacturer, Model and Version.
// They will be used in the Android application to verify that you are communicating with the correct ADK board.
// Additionnally, the URI parameter is used by the Android system to look for an appropriate Android application if none is installed.This can be a link to the Android Market or a product page.
// Ex : AndroidAccessory acc("Manufacturer","Model","Description Demo","Version","URI","Serial");
AndroidAccessory acc("0x0101011101","M2MDemoKit13", "M2M DemoKit Arduino Board - Lesson #13", "1.0", "", "0000000001234567");

// Buffer for outgoing data
byte sntmsg[3];

void setup() {

  // Set baud rate for serial communication
  Serial.begin(9600);
  
  // Set Arduino I/O #2 to OUTPUT (LED is connected to)
  pinMode(LED_ALARM_ON_PIN, OUTPUT);

  // Set Arduino I/O #5 to OUTPUT (LED is connected to)
  pinMode(LED_ALARM_OFF_PIN, OUTPUT);
   
  // Set Arduino I/O #4 to INPUT (BUTTON is connected to)
  pinMode(BUTTON_INPUT_PIN, INPUT);
    
  digitalWrite(LED_ALARM_OFF_PIN, HIGH);
  
  acc.powerOn();
}

void loop() {

  // Is something connected ?
  if (acc.isConnected()) {

    // Read current IR LED status
    irLightBarrierValue = analogRead(IR_LIGHT_BARRIER_INPUT_PIN);
  
    Serial.println(irLightBarrierValue);
    delay(1500);
  
    // Is IR LED value is superior to Threshold & Alarm is not ON ?
    if ((irLightBarrierValue < IR_LIGHT_BARRIER_THRESHOLD) && !alarm) {
      startAlarm();
    }
  
    //  Alarm ON/OFF Button management
    buttonValue = digitalRead(BUTTON_INPUT_PIN);
    Serial.println(buttonValue);
    if((buttonValue == LOW) && alarm) {
      stopAlarm();
    }
  }
}

// Start Alarm function
void startAlarm() {

  alarm = true;

  // Play sound on PIEZO
  tone(PIEZO_OUTPUT_PIN, NOTE_C7);

  // TURN OFF Alarm Green LED
  digitalWrite(LED_ALARM_OFF_PIN, LOW);
  // TURN ON Alarm Red LED
  digitalWrite(LED_ALARM_ON_PIN, HIGH);

  // Inform Android device
  sntmsg[2] = ALARM_ON;
  sendAlarmStateMessage();
}

// Stop Alarm function
void stopAlarm() {
  
  alarm = false;
  
  // Turn OFF PIEZZO BUZZER
  noTone(PIEZO_OUTPUT_PIN);
  
  // Turn OFF Alarm Red LED
  digitalWrite(LED_ALARM_ON_PIN, LOW);
  // TURN ON Alarm Green LED
  digitalWrite(LED_ALARM_OFF_PIN, HIGH);
 
  // Inform Android device
  sntmsg[2] = ALARM_OFF;
  sendAlarmStateMessage();
}

// Send ALARM status to Android device
void sendAlarmStateMessage() {
  if (acc.isConnected()) {
    sntmsg[0] = COMMAND_ALARM;
    sntmsg[1] = ALARM_TYPE_IR_LIGHT_BARRIER;
    acc.write(sntmsg,3);
  }
}
