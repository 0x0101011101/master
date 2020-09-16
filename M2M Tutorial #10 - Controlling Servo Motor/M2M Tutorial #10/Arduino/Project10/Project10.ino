// Library import
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>
// Servo.h library
#include <Servo.h>

// SERVO MOTOR params
#define COMMAND_SERVO 0x7
// Target value sent from Android
#define SERVO_ID_1 0x1
// Digital PIN on which SERVO MOTOR is connected to
#define SERVO_ID_1_PIN 2

// Move Forward & Backward command from Android
#define MOVE_FORWARD 0x1
#define MOVE_BACKWARD 0x2

// Instantiate Servo Object (Servo.h library required)
Servo servo;
int currentPos = 0;

AndroidAccessory acc("0x0101011101","M2MDemoKit10", "M2M DemoKit Arduino Board - Lesson #10", "1.0", "", "0000000001234567");

// Buffer for incoming data
byte rcvmsg[6];

void setup() {
  
  // Set baud rate for serial communication
  Serial.begin(19200);

  // Attach servo object to PIN #2 (Servo.h library required)
  servo.attach(SERVO_ID_1_PIN);
  
  // Testing Servo
  servo.write(currentPos);
  
  //Serial.print(currentPos); // Debug

  acc.powerOn();
}

void loop() {
  
  // Is something connected ?
  if(acc.isConnected()) {
    
    // Read the received data into the byte array rcvmsg[]
    int len = acc.read(rcvmsg, sizeof(rcvmsg), 1);
    
    // If something received
    if (len > 0) {
      
      // Is received bytes are assigned to SERVO MOTO on PIN #2 ?
      if (rcvmsg[0] == COMMAND_SERVO) {
        // TARGET value received from Android
        if (rcvmsg[1] == SERVO_ID_1) {

           if(rcvmsg[2] == MOVE_FORWARD) {
             if (currentPos <= 180) {
               currentPos = currentPos + 90;
             } else {
               currentPos = 0;
             }
             Serial.print(currentPos);
             servo.write(currentPos);               
             delay(150);
           }
           
           if(rcvmsg[2] == MOVE_BACKWARD) {
             if (currentPos >= -180) {
               currentPos = currentPos - 90;
             } else {
               currentPos = 0;
             }
             Serial.print(currentPos);
             servo.write(currentPos);        
             delay(150);
           }
        }
      }
    }
  }
}
    
    
