// Library import
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

// TEMPERATURE SENSOR params
// Command value sent to Android
#define COMMAND_TEMPERATURE 0x4
// Target value sent to Android
#define TARGET_VALUE 0x0
// Analogic PIN on which TEMPERATURE SENSOR is connected to
#define TEMPERATURE_SENSOR_INPUT_PIN A0

// Initialize an AndroidAccessory object which implements the Open Accessory protocol.
// Most important params are : Manufacturer, Model and Version.
// They will be used in the Android application to verify that you are communicating with the correct ADK board.
// Additionnally, the URI parameter is used by the Android system to look for an appropriate Android application if none is installed.This can be a link to the Android Market or a product page.
// Ex : AndroidAccessory acc("Manufacturer","Model","Description Demo","Version","URI","Serial");
AndroidAccessory acc("0x0101011101","M2MDemoKit8", "M2M DemoKit Arduino Board - Lesson #8", "1.0", "", "0000000001234567");

// Buffer for outgoing data
byte sntmsg[3];

void setup() {
  // Set baud rate for serial communication
  Serial.begin(19200);
  
  acc.powerOn();
}

void loop() {
  
// Is something connected ?
    if(acc.isConnected()) {
    
    // Is something connected ?
    int currentADCValue = analogRead(TEMPERATURE_SENSOR_INPUT_PIN);

    float voltage = (currentADCValue/1024.0) * 5.0;
    float temperature = (voltage - .5) * 100 - 10;
    
    sntmsg[0] = COMMAND_TEMPERATURE;
    sntmsg[1] = TARGET_VALUE;
    
    sntmsg[2] = (byte) temperature;
    
    //Serial.print(temperature);
    
    // Send buffer to Android
    acc.write (sntmsg,3);
    delay(100);
    }
}
