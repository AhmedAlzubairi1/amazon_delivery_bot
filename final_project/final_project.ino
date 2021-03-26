// These are the pins setup for the motors
int topLeftWheelPin1 = 27;     //motor1 
int topLeftWheelPin2 = 26;    //motor1
int topRightWheelPin1 = 25;     //motor2 
int topRightWheelPin2 = 33;    //motor2  
int bottomLeftWheelPin1 = 18;     //motor3 yellow
int bottomLeftWheelPin2 = 19;    //motor  3
int bottomRightWheelPin1 = 22; // Motor 4
int bottomRightWheelPin2 = 23; // Motor 4

//This is a variable to keep track of the last command the rc car was at
int last=0; // Last motion. 0 means stop, 1 means forward, 2 means back, 3 means left, 4 means right
//Source of how i used the motors is here:
//https://create.arduino.cc/projecthub/ryanchan/how-to-use-the-l298n-motor-driver-b124c5

// Set up for ultrasonic sensors
#include <UltrasonicSensor.h>
UltrasonicSensor ultrasonic(12,14);
// Set up for the peizzo buzzers
int buzzerPin=13;
bool inStop= false;

//This is the remote controller setup code stuff from exxample sketch
#include <Arduino.h>
//NOTE BUG: I need to install this h file, but not the latest version, the one before it
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

const uint16_t recvPin = 0; // Infrared receiving pin
IRrecv irrecv(recvPin);      // Create a class object used to receive class
decode_results results;       // Create a decoding results class object
//


//This is for the 7 sement LED code from the example sketch:
int dataPin = 15;          // Pin connected to DS of 74HC595（Pin14）
int latchPin = 2;          // Pin connected to ST_CP of 74HC595（Pin12）
int clockPin = 4;          // Pin connected to SH_CP of 74HC595（Pin11）

// Define the encoding of characters 0-F for the common-anode 7-Segment Display
byte num[] = {
  0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8,
  0x80, 0x90
};

//This is to remember if user is in isLooking mode
bool isLooking=false;

// This is the set up for the servers
#include <ESP32Servo.h>
Servo rightServo;
Servo leftServo;

void setup() {
  //Attach servos and set to 90 degrees
  rightServo.setPeriodHertz(50);
  rightServo.attach(5,500,2500);
  leftServo.setPeriodHertz(50);
  leftServo.attach(21,500,2500);
  leftServo.write(90);
  rightServo.write(90);
  //closeGate();
  //leftServo.write(60);
  //rightServo.write(120);
  
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(topLeftWheelPin1,OUTPUT); 
  pinMode(topLeftWheelPin2,OUTPUT);
  pinMode(topRightWheelPin1,OUTPUT); 
  pinMode(topRightWheelPin2,OUTPUT);
  pinMode(bottomLeftWheelPin1,OUTPUT); 
  pinMode(bottomLeftWheelPin2,OUTPUT);
  pinMode(bottomRightWheelPin1,OUTPUT); 
  pinMode(bottomRightWheelPin2,OUTPUT);
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
int distance = 0;
void loop() {
  // put your main code here, to run repeatedly:

   //forward();

   distance = (double)ultrasonic.distanceInCentimeters() *  0.39370;
   Serial.printf("Distance is: %din\n",distance);
   if (distance<=10 and distance>=0){
    writeDataLED(num[distance]);
    
   }
   else{
    //This is to clear the 7 segment led if I am not within distance
    writeDataLED(0xff);
   }
   
   //Check for remote communication
   if (irrecv.decode(&results)) {          // Waiting for decoding
      serialPrintUint64(results.value, HEX);// Print out the decoded results
      Serial.println("");
      handleControl(results.value);
      Serial.println("");
      irrecv.resume();                      // Release the IRremote. Receive the next value
    }
    else{
      //stopWheels();
    }
    // If in find mode, do look() method
    if (isLooking){
      look();
    }
   
}
void look(){
//Keep turning right until bot sinces object within 9 inches
  if(distance>9){
    right();
    inStop=false;
  }
  else{
    // I should stop if i find object
    stopWheels();
  
  }

  
}
// Last motion. 0 means stop, 1 means forward, 2 means back, 3 means left, 4 means right
void forward(){
  Serial.printf("FORWARD");
  Serial.println("");
  last=1;
   digitalWrite(topLeftWheelPin1,HIGH); 
   digitalWrite(topLeftWheelPin2,LOW); 
   digitalWrite(topRightWheelPin1,HIGH); 
    digitalWrite(topRightWheelPin2,LOW); 
   digitalWrite(bottomLeftWheelPin1,HIGH);
   digitalWrite(bottomLeftWheelPin2,LOW);
   digitalWrite(bottomRightWheelPin1,HIGH);
   digitalWrite(bottomRightWheelPin2,LOW);
}
void backward(){
  Serial.println("BACKWARD");
  last=2;
     digitalWrite(topLeftWheelPin1,LOW); 
   digitalWrite(topLeftWheelPin2,HIGH); 
   digitalWrite(topRightWheelPin1,LOW); 
    digitalWrite(topRightWheelPin2,HIGH); 
   digitalWrite(bottomLeftWheelPin1,LOW);
   digitalWrite(bottomLeftWheelPin2,HIGH);
   digitalWrite(bottomRightWheelPin1,LOW);
   digitalWrite(bottomRightWheelPin2,HIGH);
}
void left(){
  Serial.println("LEFT");
  last=3;

       digitalWrite(topLeftWheelPin1,LOW); 
   digitalWrite(topLeftWheelPin2,HIGH); 
      digitalWrite(topRightWheelPin1,HIGH); 
    digitalWrite(topRightWheelPin2,LOW);
       digitalWrite(bottomLeftWheelPin1,LOW);
   digitalWrite(bottomLeftWheelPin2,HIGH);
      digitalWrite(bottomRightWheelPin1,HIGH);
   digitalWrite(bottomRightWheelPin2,LOW);

  
}
void right(){
  Serial.println("RIGHT");
      last=4;

     digitalWrite(topLeftWheelPin1,HIGH); 
   digitalWrite(topLeftWheelPin2,LOW); 
      digitalWrite(topRightWheelPin1,LOW); 
    digitalWrite(topRightWheelPin2,HIGH);
       digitalWrite(bottomLeftWheelPin1,HIGH);
   digitalWrite(bottomLeftWheelPin2,LOW);
      digitalWrite(bottomRightWheelPin1,LOW);
   digitalWrite(bottomRightWheelPin2,HIGH);

}
void stopWheels(){
  Serial.println("STOP");
  last=0;
       digitalWrite(topLeftWheelPin1,LOW); 
   digitalWrite(topLeftWheelPin2,LOW); 
      digitalWrite(topRightWheelPin1,LOW); 
    digitalWrite(topRightWheelPin2,LOW);
       digitalWrite(bottomLeftWheelPin1,LOW);
   digitalWrite(bottomLeftWheelPin2,LOW);
      digitalWrite(bottomRightWheelPin1,LOW);
   digitalWrite(bottomRightWheelPin2,LOW);
}
void buzzer(boolean turnOn){
  if (turnOn){
    digitalWrite(buzzerPin, HIGH); 
  }
  else{
    digitalWrite(buzzerPin,LOW);
  }
}
//This is to keep track of the last motion to be used to continue the motion
void lastMotion(){
  if (last==1){
    forward();
  }
  else if (last==2){
    backward();
  }
  else if (last==3){
    left();
  }
  else if (last==4){
    right();
  }
  else{
    stopWheels();
  }
}
//This code was taking from example. The mechaism to control the remote that is. 
//NOTE: Have the Reciever clearly exposed so it is clear the signal can go to it.
void handleControl(unsigned long value) {
  // Make a sound when it rereives commands
  // Handle the commands
  buzzer(true);
  delay(50);
  buzzer(false);
  switch (value) {
    case 0xFF02FD:// Pressed +
      isLooking=false;
      forward();
      break;
    case 0xFFE01F:              // Pressed reverse
      isLooking=false;
      left();
      break;
    case 0xFF9867:              // Pressed -
      isLooking=false;
      backward();
      break;
    case 0xFF906F:              // Pressed forward
      isLooking=false;
      right();
      break;
    case 0xFFA857: // Pressed play
      isLooking=false;
      stopWheels();
      break;
    case 0xFFA25D: // This is for look, press power button
      isLooking=!isLooking;
      inStop=false;
      //If i turned it off, stop the wheels
      if (!isLooking){
        stopWheels();
      }
      break;
    case 0xFF6897: // 0 pressed, so do open
     openGate();
     break;
     case 0xFFB04F: // C pressed, so I want to close
     closeGate();
     break;
    // These are the slow directions:
    case 0xFF18E7: //pressed 2, so slow forward
      isLooking=false;
      forward();
      delay(200);
      stopWheels();
      break;
    case 0xFF10EF: // Pressed 4, so slow left
      isLooking=false;
      left();
      delay(200);
      stopWheels();
      break;
   case 0xFF4AB5: // Pressed 8, so slow back
      isLooking=false;
      backward();
      delay(200);
      stopWheels();
      break;
  case 0xFF5AA5: // Pressed 6, so slow right
      isLooking=false;
      right();
      delay(200);
      stopWheels();
      break;
    //If i get weird communication, should just do last motion method
    default:
        if (!isLooking){
          lastMotion();
        }
  }
}
//This is to write to led. SOURCED from example sketch
void writeDataLED(int value) {
  // Make latchPin output low level
  digitalWrite(latchPin, LOW);
  // Send serial data to 74HC595
  shiftOut(dataPin, clockPin, LSBFIRST, value);
  // Make latchPin output high level, then 74HC595 will update the data to parallel output
  digitalWrite(latchPin, HIGH);
}
void openGate(){
  //Set the servo angel to drop the crane
  Serial.println("Open");
  leftServo.write(90);
  rightServo.write(90);
  
}
void closeGate(){
//Set servo angel to lift the crane
  Serial.println("Close");
  leftServo.write(60);
  rightServo.write(120);
  
  
}
