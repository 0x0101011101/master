// Library import
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

// POTIENTOMETER params
// Command value received from Android
#define COMMAND_ANALOG 0x3
// Target value received from Android
#define TARGET_PIN 0x0

// Arduino Board Analog PIN Number on which POTIENTOMETER is connected to
#define INPUT_PIN A0

// Initialize an AndroidAccessory object which implements the Open Accessory protocol.
// Most important params are : Manufacturer, Model and Version.
// They will be used in the Android application to verify that you are communicating with the correct ADK board.
// Additionnally, the URI parameter is used by the Android system to look for an appropriate Android application if none is installed.This can be a link to the Android Market or a product page.
// Ex : AndroidAccessory acc("Manufacturer","Model","Description Demo","Version","URI","Serial");
AndroidAccessory acc("0x0101011101","M2MDemoKit4", "M2M DemoKit Arduino Board - Lesson #4", "1.0", "", "0000000001234567");

// Buffer for incoming data
byte sntmsg[6];

// To read the current Potentiometer state
int analogPinReading;

void setup() {
  // Set baud rate for serial communication
  Serial.begin(19200);
  
  acc.powerOn();
  
  // Initialize buffer value
  sntmsg[0] = COMMAND_ANALOG;
  sntmsg[1] = TARGET_PIN;
}

void loop() {
  
  // Is something connected ?
  if(acc.isConnected()) {
    
    // Read the current Potentiometer state on PIN #A0
    analogPinReading = analogRead(INPUT_PIN);
    
    // Int to Byte Conversion
    sntmsg[2] = (byte) (analogPinReading >> 24);
    sntmsg[3] = (byte) (analogPinReading >> 16);
    sntmsg[4] = (byte) (analogPinReading >> 8);
    sntmsg[5] = (byte) analogPinReading;
    
    acc.write(sntmsg,6); // Send Potientometer value to Android
    delay(100);
  }
}
