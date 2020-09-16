/*
  Example Bluetooth Serial Passthrough Sketch
 by: Jim Lindblom
 SparkFun Electronics
 date: February 26, 2013
 license: Public domain

 This example sketch converts an RN-42 bluetooth module to
 communicate at 9600 bps (from 115200), and passes any serial
 data between Serial Monitor and bluetooth module.
 */

 /*
  * Type $$$
  * In console, choose "No line ending", 9600 Bauds
  * Type 'D' to check your Bluetooth device configuration
  * Type 'E' to check your Bluetooth device extended configuration
  * In console, choose "newline"
  * Type 'I,10' to scan all Bluetooth devices
  * Type 'C,<BTA number>' to connect to the chosen Bluetooth Device (You will find its BTA number w/ previous scanning command)
  * Be sure to have pairing the Arduino w/ your Smartphone before runing
  */
  
#include <SoftwareSerial.h>  

const int bluetoothTx = 2;  // TX-O pin of bluetooth mate, Arduino D2
const int bluetoothRx = 3;  // RX-I pin of bluetooth mate, Arduino D3
const int greenLed = 13; // Define Test Led pin

const int buttonPin = 8; // Define Button pin
int buttonState = 0; // variable for reading the pushbutton status
int buttonCountPressed = 0;
boolean oddOrEven = false;

// Initialisation Connexion between Arduino & BlueSmirf Module
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

// Var to analyze which char has been sent by the Smartphone
char openLed = 'o';
char charReceived;

void setup()
{
  pinMode(13,OUTPUT); // Set Pin#13 in OUTPUT mode
  pinMode(buttonPin, INPUT ); // initialize the pushbutton pin as an input:
  
  Serial.begin(9600);  // Begin the serial monitor at 9600bps

  bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
  bluetooth.print("$");  // Print three times individually
  bluetooth.print("$");
  bluetooth.print("$");  // Enter command mode
  delay(100);  // Short delay, wait for the Mate to send back CMD
  bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
  bluetooth.begin(9600);  // Start bluetooth serial at 9600

}

void loop()
{
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  
  if (buttonState == HIGH) {

    buttonCountPressed++;
    oddOrEven = buttonCountPressed % 2;

    if (oddOrEven) {
      digitalWrite(greenLed,HIGH);
      bluetooth.println("Button pressed / Light ON");
    }
    
    if (!oddOrEven){
      digitalWrite(greenLed,LOW);
      bluetooth.println("Button pressed / Light OFF"); 
    }
  }

  // Received from Smarpthone
  if(bluetooth.available())  // If the bluetooth sent any characters
  {
    charReceived = bluetooth.read();
    
    // Send any characters the bluetooth prints to the serial monitor
    Serial.print(charReceived);

    // Led command received ? 
    if (charReceived == 'o') {
      digitalWrite(greenLed,HIGH);
    } else {
      digitalWrite(greenLed,LOW);
    }
  }

  // Sent from Arduino
  if(Serial.available())  // If stuff was typed in the serial monitor
  {
    // Send any characters the Serial monitor prints to the bluetooth
    bluetooth.print((char)Serial.read());
    
  }
  // and loop forever and ever!
}
