// Library import
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

// PIEZO BUZZER params
// Command value received from Android
#define COMMAND_ANALOG 0x3
// Target value received from Android
#define TARGET_PIN_2 0x2

// Initialize an AndroidAccessory object which implements the Open Accessory protocol.
// Most important params are : Manufacturer, Model and Version.
// They will be used in the Android application to verify that you are communicating with the correct ADK board.
// Additionnally, the URI parameter is used by the Android system to look for an appropriate Android application if none is installed.This can be a link to the Android Market or a product page.
// Ex : AndroidAccessory acc("Manufacturer","Model","Description Demo","Version","URI","Serial");
AndroidAccessory acc("0x0101011101","M2MDemoKit5", "M2M DemoKit Arduino Board - Lesson #5", "1.0", "", "0000000001234567");

// Buffer for incoming data
byte rcvmsg[6];

void setup() {
  // Set baud rate for serial communication
  Serial.begin(19200);
  
  // Set I/O #2 to OUTPUT
  pinMode(TARGET_PIN_2, OUTPUT);
  
  acc.powerOn();
}

void loop() {
  
  // Is something connected ?
  if(acc.isConnected()) {
    
    // Read the received data into the byte array rcvmsg[]
    int len = acc.read(rcvmsg, sizeof(rcvmsg),1);
    
    // If something received
    if (len > 0) {
      
      // Is received bytes are assigned to PIEZO BUZZER on PIN #2 ? 
      if (rcvmsg[0] == COMMAND_ANALOG) {
        if (rcvmsg[1] == TARGET_PIN_2) {
          
          // Convert Byte to Int
          int output = ((rcvmsg[2] & 0xFF) << 24)
                       + ((rcvmsg[3] & 0xFF) << 16)
                       + ((rcvmsg[4] & 0xFF) << 8)
                       + (rcvmsg[5] & 0xFF);
                       
          // Set the frequency for the desired tone in Hz
          tone(TARGET_PIN_2, output);
        }
      }
    }
  }
}
