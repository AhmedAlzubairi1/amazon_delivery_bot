int outPin1 = 26;     //motor1 
int outPin2 = 27;    //motor1 
//https://create.arduino.cc/projecthub/ryanchan/how-to-use-the-l298n-motor-driver-b124c5
// wwatch the video  attached
#include <UltrasonicSensor.h>
UltrasonicSensor ultrasonic(12,14);
int buzzerPin=13;


//This is the remote stuff from exxample sketch
#include <Arduino.h>
//NOTE BUG: I need to install this h file, but not the latest version, the one before it
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

const uint16_t recvPin = 0; // Infrared receiving pin
IRrecv irrecv(recvPin);      // Create a class object used to receive class
decode_results results;       // Create a decoding results class object
//


//This is for the 8 digit led:
int dataPin = 15;          // Pin connected to DS of 74HC595（Pin14）
int latchPin = 2;          // Pin connected to ST_CP of 74HC595（Pin12）
int clockPin = 4;          // Pin connected to SH_CP of 74HC595（Pin11）

// Define the encoding of characters 0-F for the common-anode 7-Segment Display
byte num[] = {
  0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8,
  0x80, 0x90
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
pinMode(outPin1,OUTPUT); 
pinMode(outPin2,OUTPUT);
  //Set ultra sonic sensor to room temperature to be more acurate
  ultrasonic.setTemperature(22);
  pinMode(buzzerPin,OUTPUT);
  //Enable the remote
  irrecv.enableIRIn();//Start the reciever remote

  //Set up for the 8LED
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

   //digitalWrite(outPin1,LOW); 
   //digitalWrite(outPin2,HIGH); 
   int distance = ultrasonic.distanceInCentimeters();
   Serial.printf("Distance is: %dcm\n",distance);
   buzzer(false);
   if (irrecv.decode(&results)) {          // Waiting for decoding
      //serialPrintUint64(results.value, HEX);// Print out the decoded results
      //Serial.println("");
      handleControl(results.value);
      Serial.println("");
      irrecv.resume();                      // Release the IRremote. Receive the next value
    }
    //This is to just print out the LED lights in sequence
   for (int i = 0; i < 10; i++) {
    writeDataLED(num[i]);// Send data to 74HC595
    delay(1000);      // delay 1 second
    writeDataLED(0xff);  // Clear the display content
  }
  //delay(1000);   
}
void buzzer(boolean turnOn){
  if (turnOn){
    digitalWrite(buzzerPin, HIGH); 
  }
  else{
    digitalWrite(buzzerPin,LOW);
  }
}
//This code was taking from example. The mechaism to control the remote that is. 
//NOTE: Have the Reciever exposed so it is clear the signal can go to it.
void handleControl(unsigned long value) {
  // Make a sound when it rereives commands
  // Handle the commands
  switch (value) {
    case 0xFF6897:// Receive the number '0'
      Serial.printf("Pressed 0");          // Turn off LED
  buzzer(true);
  delay(100);
  buzzer(false);
      break;
    case 0xFF30CF:              // Receive the number '1'
      Serial.printf("Pressed 1");          // Dimmest brightness
      break;
    case 0xFF18E7:              // Receive the number '2'
      Serial.printf("Pressed 2");         // Medium brightness
      break;
    case 0xFF7A85:              // Receive the number '3'
      Serial.printf("Pressed 3");        // Strongest brightnss
      break;
  }
}
//This is to write to led. SOURCED from example sketch
//writeData(0xff);  // Clear the display content
void writeDataLED(int value) {
  // Make latchPin output low level
  digitalWrite(latchPin, LOW);
  // Send serial data to 74HC595
  shiftOut(dataPin, clockPin, LSBFIRST, value);
  // Make latchPin output high level, then 74HC595 will update the data to parallel output
  digitalWrite(latchPin, HIGH);
}
