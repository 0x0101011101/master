// Library import
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

// PIEZO BUZZER params
// Command value received from Android
#define COMMAND_ANALOG 0x3
// Target value received from Android
#define INPUT_PIN_0 0x0

// Initialize an AndroidAccessory object which implements the Open Accessory protocol.
// Most important params are : Manufacturer, Model and Version.
// They will be used in the Android application to verify that you are communicating with the correct ADK board.
// Additionnally, the URI parameter is used by the Android system to look for an appropriate Android application if none is installed.This can be a link to the Android Market or a product page.
// Ex : AndroidAccessory acc("Manufacturer","Model","Description Demo","Version","URI","Serial");
AndroidAccessory acc("0x0101011101","M2MDemoKit6", "M2M DemoKit Arduino Board - Lesson #6", "1.0", "", "0000000001234567");

// Buffer for outgoing data
byte sntmsg[6];

void setup() {
  // Set baud rate for serial communication
  Serial.begin(19200);
  
  acc.powerOn();
  
  sntmsg[0] = COMMAND_ANALOG;
  sntmsg[1] = INPUT_PIN_0;
}

void loop() {

  // Is something connected ?
  if(acc.isConnected()) {

    // Read PIEZO BUZZER Analog Value
    int currentValue = analogRead(INPUT_PIN_0);

    // Convert Int to Byte
    sntmsg[2] = (byte) (currentValue >> 24);
    sntmsg[3] = (byte) (currentValue >> 16);
    sntmsg[4] = (byte) (currentValue >> 8);
    sntmsg[5] = (byte) currentValue;
    
    // Send to Android
    acc.write (sntmsg,6);
    delay(100);
  }
}
