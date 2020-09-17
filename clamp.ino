/* Authors: Dilan Lahkani, Michelle Loven 
 * Created: March 20, 2019
 * Last edited: July 26, 2019
 * 
 * created by Rui Santos, https://randomnerdtutorials.com
 * 
 * Complete Guide for Ultrasonic Sensor HC-SR04
 *
    Ultrasonic sensor Pins:
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin11
        Echo: Echo (OUTPUT) - Pin 12
        GND: GND
 */
#include <Servo.h>
int trigPin = 9;    // Trigger
int echoPin = 46;    // Echo
int servoPin = A3; //analog pin for servo
int button1 = 48; //digital pin for first button
int button2 = 50; //digital pin for second button
long duration, cm, inches; 
int val1 = 0;
int val2 = 0;
Servo myservo;

int stage = 2;
int stagePin = 51;

const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
int counter = 0;
int counter2 = 0;


void setup() {
  //Serial Port begin
  Serial.begin (9600);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(button1,INPUT);
  pinMode(button2,INPUT);
  pinMode(stagePin,INPUT);
  myservo.attach(servoPin);
  total = 0;
  average = 0;
  for (int thisReading = 0; thisReading < numReadings; thisReading++){
    readings[thisReading] = 0;
  }
}

void ultrasonic() {
    // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  // Convert the time into a distance
  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  getAverage();
}


void getAverage(){
  // subtract the last reading:
    total = total - readings[readIndex];
    // read from the sensor:
    readings[readIndex] = inches;
    // add the reading to the total:
    total = total + readings[readIndex];
    // advance to the next position in the array:
    readIndex++;
  
    // if we're at the end of the array
    if (readIndex >= numReadings) {
      //wrap around to the beginning
      readIndex = 0;
    }
  
    // calculate the average:
    average = total / numReadings;
  
}

//close = 1
//open = 0

void checkButton(){
  while(digitalRead(stagePin) == 0){
    openClamp();
  }
}

void openClamp(){
  myservo.write(150);
}


void loop() {
  checkButton();
    if(counter<=500){
      ultrasonic();
      delay(10);
      checkButton();
      counter++;
    }
    if(counter>=500){
      checkButton();
      val1 = digitalRead(button1);
      val2 = digitalRead(button2);
  
       if(average < 4){
        checkButton();
        //WHILE EITHER BUTTON IS NOT PRESSED
        while ((val1 == 0 or val2 == 0)){
          checkButton();
          myservo.write(60);              //closes clamp
          ultrasonic();                   //update ultrasonic reading
          val1 = digitalRead(button1);    //update buttons
          val2 = digitalRead(button2);
          Serial.println("Delayed close: Average: " + String(average)); 
         // Serial.println("Average: " + String(average));
         // Serial.println("Inches: " + String(inches));
        }
       }
      //WHILE BOTH BUTTONS ARE PRESSED
      while (val1 == 1 and  val2 == 1){
        if (counter2 <= 300){
           myservo.write(60);
           counter2++;
           delay(1);
           checkButton();
           //Serial.println("Delay of clamp");
        }
        else{
          myservo.write(95); //stops rotation
          val1 = digitalRead(button1); //update buttons
          val2 = digitalRead(button2);
          Serial.println("Clamp is tight");
          checkButton();
        }
        Serial.println(counter2);
        checkButton();
      }
    
      ultrasonic();
      if (average < 7){
        myservo.write(60);
        checkButton();
      }
      else{
        checkButton();
        myservo.write(95);
      }
      checkButton();
      Serial.print(average);
      Serial.print("in");
      Serial.println();
      delay(250);
    }
}
