// Includes would be placed here
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

// Constant definition
#define ARRAY_SIZE 27
#define COMMAND_TEXT 0xF
#define TARGET_DEFAULT 0xF

// Initialize an AndroidAccessory object which implements the Open Accessory protocol.
// Most important params are : Manufacturer, Model and Version.
// They will be used in the Android application to verify that you are communicating with the correct ADK board.
// Additionnally, the URI parameter is used by the Android system to look for an appropriate Android application if none is installed.This can be a link to the Android Market or a product page.
// Ex : AndroidAccessory acc("Manufacturer","Model","Description Demo","Version","URI","Serial");
AndroidAccessory acc("0x0101011101","M2MAndroidArduinoHelloWorld", "M2M DemoKit Arduino Board - Hello World", "1.0", "", "0000000001234567");

// Global variable definition
char hello[ARRAY_SIZE] = {'H','e','l','l','o',' ','W','o','r','l','d',' ','f','r','o','m',' ','A','r','d','u','i','n','o',' ','!'};

byte rcvmsg[255];
byte sntmsg[3 + ARRAY_SIZE];

void setup() {
  // Set baudrate for serial communication
  Serial.begin(115200);
  acc.powerOn();
}

void loop() {
  if(acc.isConnected()) {
    // Read the sent text message into the byte array
    int len = acc.read(rcvmsg, sizeof(rcvmsg),1);
    if (len > 0) {
      if(rcvmsg[0] == COMMAND_TEXT) {
        if(rcvmsg[1]  == TARGET_DEFAULT) {
          // Get the textLength from the checksum byte
          byte textLength = rcvmsg[2];
          int textEndIndex = 3 + textLength;
          // print each character to the serial output
          for (int x = 3; x < textEndIndex; x++) {
            Serial.print((char) rcvmsg[x]);
            delay(250);
          }
          Serial.println();
          delay(250);
        }
      }
    }
    
    sntmsg[0] = COMMAND_TEXT;
    sntmsg[1] = TARGET_DEFAULT;
    sntmsg[2] = ARRAY_SIZE;
    
    // Print characters from array to serial monitor
    for (int x = 0; x < ARRAY_SIZE; x++) {
      sntmsg[3 + x] = hello[x];
    }
    
    acc.write(sntmsg, 3 + ARRAY_SIZE);
    delay(250);
  }
}
