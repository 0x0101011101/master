// Library import
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

// DC MOTOR params
#define COMMAND_DC_MOTOR 0x8
// Target value sent from Android
#define DC_MOTOR_ID_1 0x1
// Digital PIN on which DC MOTOR is connected to
#define DC_MOTOR_ID_1_PIN 2

// Initialize an AndroidAccessory object which implements the Open Accessory protocol.
// Most important params are : Manufacturer, Model and Version.
// They will be used in the Android application to verify that you are communicating with the correct ADK board.
// Additionnally, the URI parameter is used by the Android system to look for an appropriate Android application if none is installed.This can be a link to the Android Market or a product page.
// Ex : AndroidAccessory acc("Manufacturer","Model","Description Demo","Version","URI","Serial");
AndroidAccessory acc("0x0101011101","M2MDemoKit11", "M2M DemoKit Arduino Board - Lesson #11", "1.0", "", "0000000001234567");

// Buffer for incoming data
byte rcvmsg[3];
 
void setup() {

  // Set baud rate for serial communication
  Serial.begin(19200);
  // Set Arduino I/O #2 to OUTPUT (DC MOTOR is connected to)
  pinMode(DC_MOTOR_ID_1_PIN, OUTPUT);
  acc.powerOn();
}

void loop() {
 
  // Is something connected ?
  if(acc.isConnected()) {
    
    // Read the received data into the byte array rcvmsg[]
    int len = acc.read(rcvmsg, sizeof(rcvmsg), 1);
    
    // If something received
    if (len > 0) {
      
      // Is received bytes are assigned to LED on PIN #2 ?
      if (rcvmsg[0] == COMMAND_DC_MOTOR) {
        if (rcvmsg[1] == DC_MOTOR_ID_1) {

          // Get the SeekBar value from Android
          int motorSpeed = rcvmsg[2] & 0xFF;
          
          //Serial.print(motorSpeed);
          
          // Mapping the received value from Android
          motorSpeed = map(motorSpeed, 0, 100, 0, 255);
          
          // Send DC MOTOR speed to I/O Pin
          digitalWrite(DC_MOTOR_ID_1_PIN, motorSpeed);
        }
      }
    }
  }
}
