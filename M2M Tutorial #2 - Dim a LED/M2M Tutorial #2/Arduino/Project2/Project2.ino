// Library import
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

// LED params
// Command value received from Android
#define COMMAND_LED 0x2
// Target value received from Android
#define TARGET_PIN_2 0x2

// Arduino Board PIN Number on which LED is connected to
#define LED_PIN 2


// Initialize an AndroidAccessory object which implements the Open Accessory protocol.
// Most important params are : Manufacturer, Model and Version.
// They will be used in the Android application to verify that you are communicating with the correct ADK board.
// Additionnally, the URI parameter is used by the Android system to look for an appropriate Android application if none is installed.This can be a link to the Android Market or a product page.
// Ex : AndroidAccessory acc("Manufacturer","Model","Description Demo","Version","URI","Serial");
AndroidAccessory acc("0x0101011101","M2MDemoKit2", "M2M DemoKit Arduino Board - Lesson #2", "1.0", "", "0000000001234567");

// Buffer for incoming data
byte rcvmsg[3];

void setup() {
  // Set baud rate for serial communication
  Serial.begin(19200);
  
  acc.powerOn();
  
  // Set Arduino I/O #2 to OUTPUT (LED is connected to)
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  
  // Is something connected ?
  if(acc.isConnected()) {
    
    // Read the received data into the byte array rcvmsg[]
    int len = acc.read(rcvmsg, sizeof(rcvmsg),1);
    
    // If something received
    if (len > 0) {
      
      // Is received bytes are assigned to LED on PIN #2 ?      
      if(rcvmsg[0] == COMMAND_LED) {
        if(rcvmsg[1]  == TARGET_PIN_2) {
          // Get the SeekBar value from Android
          byte value = rcvmsg[2];
          // Dim LED depending on actual Android SeekBar value
          analogWrite(LED_PIN, value);
        }
      }
    }
  }
}
