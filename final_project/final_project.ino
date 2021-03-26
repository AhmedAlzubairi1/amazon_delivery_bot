int topLeftWheelPin1 = 27;     //motor1 
int topLeftWheelPin2 = 26;    //motor1
int topRightWheelPin1 = 25;     //motor2 
int topRightWheelPin2 = 33;    //motor2  
int bottomLeftWheelPin1 = 18;     //motor3 yellow
int bottomLeftWheelPin2 = 19;    //motor  3
int bottomRightWheelPin1 = 22; // Motor 4
int bottomRightWheelPin2 = 23; // Motor 4
int last=0; // Last motion. 0 means stop, 1 means forward, 2 means back, 3 means left, 4 means right
//https://create.arduino.cc/projecthub/ryanchan/how-to-use-the-l298n-motor-driver-b124c5
// wwatch the video  attached
#include <UltrasonicSensor.h>
UltrasonicSensor ultrasonic(12,14);
int buzzerPin=13;
bool inStop= false;

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

bool combatMode=false;

#include <ESP32Servo.h>
Servo rightServo;
Servo leftServo;

void setup() {
  //Attach servos
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
//   Serial.printf("Distance is: %din\n",distance);
   if (distance<=10 and distance>=0){
  //  Serial.printf("Showing %d\n",distance);
    writeDataLED(num[distance]);
    
   }
   else{
   // Serial.printf("Turn off \n");
    writeDataLED(0xff);
   }
   
   
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
    if (combatMode){
      combat();
    }
   /*
   // This is to just print out the LED lights in sequence
   for (int i = 0; i < 10; i++) {
    writeDataLED(num[i]);// Send data to 74HC595
    delay(1000);      // delay 1 second
    writeDataLED(0xff);  // Clear the display content
  }
  delay(1000);   
  */
  //delay(1000);
  
}
void combat(){
  if(distance>9){
    right();
    inStop=false;
  }
  else{
    // I should stop if i find it. 
    stopWheels();
    /*
    if(inStop){
      forward(); 
    }
    else{
      stopWheels();
      inStop=true;
    }
    */
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
//NOTE: Have the Reciever exposed so it is clear the signal can go to it.
void handleControl(unsigned long value) {
  // Make a sound when it rereives commands
  // Handle the commands
  buzzer(true);
  delay(50);
  buzzer(false);
  switch (value) {
    case 0xFF02FD:// Pressed +
      combatMode=false;
      forward();
      break;
    case 0xFFE01F:              // Pressed reverse
      combatMode=false;
      left();
      break;
    case 0xFF9867:              // Pressed -
      combatMode=false;
      backward();
      break;
    case 0xFF906F:              // Pressed forward
      combatMode=false;
      right();
      break;
    case 0xFFA857: // Pressed play
      combatMode=false;
      stopWheels();
      break;
    case 0xFFA25D: // This is for combat, press power button
      combatMode=!combatMode;
      inStop=false;
      //If i turned it off, stop the wheels
      if (!combatMode){
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
      combatMode=false;
      forward();
      delay(200);
      stopWheels();
      break;
    case 0xFF10EF: // Pressed 4, so slow left
      combatMode=false;
      left();
      delay(200);
      stopWheels();
      break;
   case 0xFF4AB5: // Pressed 8, so slow back
      combatMode=false;
      backward();
      delay(200);
      stopWheels();
      break;
  case 0xFF5AA5: // Pressed 6, so slow right
      combatMode=false;
      right();
      delay(200);
      stopWheels();
      break;

    
    default:
        if (!combatMode){
          lastMotion();
        }
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
void openGate(){
  Serial.println("Open");
  leftServo.write(90);
  rightServo.write(90);
  
}
void closeGate(){
  /*
  int temp=10;
  //60,120
  while (temp<40){
  leftServo.write(90-temp);
  rightServo.write(90+temp);
  // Add delay so it doesnt close too fast
  delay(200);
  temp+=10;
   
  }
  */
  Serial.println("Close");
  leftServo.write(60);
  rightServo.write(120);
  
  
}
