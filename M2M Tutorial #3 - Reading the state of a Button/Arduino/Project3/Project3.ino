// Library import
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

// SWITCH params
// Command value received from Android
#define COMMAND_BUTTON 0x1
// Target value received from Android
#define TARGET_BUTTON 0x1
#define VALUE_ON 0x1
#define VALUE_OFF 0x0

// Arduino Board PIN Number on which SWITCH is connected to
#define INPUT_PIN 2

// Initialize an AndroidAccessory object which implements the Open Accessory protocol.
// Most important params are : Manufacturer, Model and Version.
// They will be used in the Android application to verify that you are communicating with the correct ADK board.
// Additionnally, the URI parameter is used by the Android system to look for an appropriate Android application if none is installed.This can be a link to the Android Market or a product page.
// Ex : AndroidAccessory acc("Manufacturer","Model","Description Demo","Version","URI","Serial");
AndroidAccessory acc("0x0101011101","M2MDemoKit3", "M2M DemoKit Arduino Board - Lesson #3", "1.0", "", "0000000001234567");

// Buffer for incoming data
byte sntmsg[3];
int lastButtonState;
int currentButtonState;

void setup() {
  // Set baud rate for serial communication
  Serial.begin(19200);
  
  acc.powerOn();
  
  // Initialize buffer value
  sntmsg[0] = COMMAND_BUTTON;
  sntmsg[1] = TARGET_BUTTON;
}

void loop() {
  
  // Is something connected ?
  if(acc.isConnected()) {
    
    // Read the current Switch state on PIN #2
    currentButtonState = digitalRead(INPUT_PIN);
    
    // Compare current state with previous state
    if(currentButtonState != lastButtonState) {
      if(currentButtonState == LOW) {
        sntmsg[2] = VALUE_ON;
      } else {
        sntmsg[2] = VALUE_OFF;
      }
      acc.write(sntmsg,3); // Send Switch state to Android
      lastButtonState = currentButtonState;
    }
    delay(100);
  }
}
