// Library import
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

// Library import Capacitive Sensor
#include <CapacitiveSensor.h>

// CAPACITIVE SENSOR params
#define COMMAND_TOUCH_SENSOR 0x6
// Target value sent to Android
#define SENSOR_ID 0x0
// Capacitive Sensor from which measurements are sent
#define THRESHOLD 50

// Instantiate Capacitive Sensor Object from Capacitive Library (Arduino I/O port used are 4,6)
CapacitiveSensor touchSensor = CapacitiveSensor(4,6);

// Initialize an AndroidAccessory object which implements the Open Accessory protocol.
// Most important params are : Manufacturer, Model and Version.
// They will be used in the Android application to verify that you are communicating with the correct ADK board.
// Additionnally, the URI parameter is used by the Android system to look for an appropriate Android application if none is installed.This can be a link to the Android Market or a product page.
// Ex : AndroidAccessory acc("Manufacturer","Model","Description Demo","Version","URI","Serial");
AndroidAccessory acc("0x0101011101","M2MDemoKit9", "M2M DemoKit Arduino Board - Lesson #9", "1.0", "", "0000000001234567");

// Buffer for outgoing data
byte sntmsg[3];

void setup() {
  // Set baud rate for serial communication
  Serial.begin(19200);
  
  acc.powerOn();
  
  // Disables Auto Calibration
  touchSensor.set_CS_AutocaL_Millis(0xFFFFFFFF);
  
  // Initialize buffer with Touch Sensor params
  sntmsg[0] = COMMAND_TOUCH_SENSOR;
  sntmsg[1] = SENSOR_ID;
}

void loop() {
  
  // Is something connected ?
  if(acc.isConnected()) {
    
    // Takes 30 measurements to reduce false readings and disturbances
    long value = touchSensor.capacitiveSensor(30);
    
    // Touch Sensor value is superior to fixed THRESHOLD > Send 1
    if (value > THRESHOLD) {
      sntmsg[2] = 0x1;
    } else { // Else send 0
      sntmsg[2] = 0x0;
    }
    
    // Send buffer to Android
    acc.write (sntmsg,3);
    delay(100);
  }
}
