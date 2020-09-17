/***
  Author: Michelle Loven, Dilan Lahkani, Arpit Kalla
  Date: May 31, 2019
  Last edited: July 3, 2019
  
  Based off project by Rui Santos
  Complete project details at http://randomnerdtutorials.com
***/

//Setting up wifi 
#include <WiFi.h>
#include <ESP32_Servo.h>
WiFiServer server(80);
WiFiClient httpClient;
const char* ssid     = "RPAL";
const char* password = "rpalrpal";

//Sensor pins
static const int servoPin = 12;
static const int buttonPin = 27;
static const int topServo = 33;

int sensorpin = A3;                 // variable to store the values from sensor(initially zero)
int threshold = 25; 
int counter = 0;  
int sideButton1 = 15;
int sideButton2 = 32;
int sideButton1Data = 1;
int sideButton2Data = 1;  

//Averaging algo for sensor readings
const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

// Variable to store the HTTP request
String header;

// Decode HTTP GET value
String valueString = String(5);
int pos1 = 0;
int pos2 = 0;
String message = "";

//Determining location of hole
int state = 0;

//Working with the button
int counterLoop = 1;
int val = 0;                        // variable to store the values from sensor(initially zero)
int old_val = 0;                    //remembers last value to compare to new one

//Working with the servos
Servo servoTwo;  
Servo screwServo;

int startState = 0;


void setup() {
  Serial.begin(115200);
  turnWifiOn();
  while(startState == 0){
//    post("Starting...");
    getData();
  }
  setup3();
}

//RESET SETUP
void setup2(){
  total = 0;
  average = 0;
  for (int thisReading = 0; thisReading < numReadings; thisReading++){
    readings[thisReading] = 0;
  }
}

//START SETUP
void setup3(){
  total = 0;
  average = 0;
  servoTwo.attach(servoPin);               // attaches the servo on the servoPin to the servo object
  screwServo.attach(topServo);
  old_val=analogRead(sensorpin);     //initializes old_val to first reading    // attaches the servo on the servoPin to the servo object
  //initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++){
    readings[thisReading] = 0;
  }
  pinMode(sideButton1, INPUT);
  pinMode(sideButton2, INPUT);

  startState = 0;
}

void startServo(int num){
  servoTwo.write(num);
}

/**
 * Turning wifi capabilities on and start the server 
 */
void turnWifiOn(){
//  Serial.print("Connecting to ");
//  Serial.println(ssid);
  WiFi.begin(ssid,password); //initializes WiFi connection
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//    Serial.print(".");
  }
  
//  Serial.println("");
//  Serial.println("WiFi connected.");
//  Serial.println("IP address: ");
//  Serial.println(WiFi.localIP());
  server.begin();     //creates server

}

/**
 * Recieving commands from get requests, sends to appropriate
 * methods
 * command: string recieved from get request
 */
void commandServo(String command){
  if(command == "up"){
    servoTwo.write(0);
    delay(200);
    servoTwo.write(80);  // the servo stops moving
    } 
  else if (command == "down"){
    servoTwo.write(180);
    delay(200);
    servoTwo.write(80);  // the servo stops moving
    }
  else if (command == "screwdown"){
     servoTwo.write(97);
     screwServo.write(45);
     delay(200);
    servoTwo.write(80);
    }
  else if(command == "screwup"){
    screwServo.write(110);
    servoTwo.write(50);
    delay(200);
    servoTwo.write(80);
  }
  else if(command == "reset"){
    post("No value"); //equivalent of pressing button to reset
    state = 1;        //funnel and sensor
  }
  else if(command == "start"){
    startState = 1;
  }
}

void post(String message){
  httpClient = server.available();            // Listen for incoming clients
  httpClient.println(message);
}

void getData(){
  WiFiClient client = server.available();
  if (client) { 
//    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                  // make a String to hold incoming data from the client
    while (client.connected()) {              // loop while the client's connected
      
      if (client.available()) { 
        char c = client.read();               // read a byte, then
        header += c;
        message = header;
        if (c == '\n') {  
          if (currentLine.length() == 0) {
            
            //GET /?value=180& HTTP/1.1
            if(header.indexOf("GET /?value=")>=0) {
              
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              
              //Rotate the servo
              int spaceIndex = valueString.indexOf(' ');
              String servoCommand = valueString.substring(0,spaceIndex);

              commandServo(servoCommand);
         
            }       
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
     //Close the connection
    client.stop();
    post("LOADING...");
//    Serial.println("Client disconnected.");
//    Serial.println("");
  }
}


double getAverage(){
  // subtract the last reading:
    total = total - readings[readIndex];
    // read from the sensor:
    readings[readIndex] = analogRead(sensorpin);
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
    int average = total / numReadings;
    return average;
}

/**
 * Loops through this code
 */
void loop(){
    state = digitalRead(buttonPin);
    delay(1);        // delay in between reads for stability
//    Serial.println(getAverage());
    getAverage();
    val=getAverage();
//    while (sideButton1 == 0 or sideButton2 == 0){
//      sideButton1 = digitalRead(sideButton1);
//      sideButton2 = digitalRead(sideButton2);
//      post("No value"); //equivalent of pressing button to reset
//      state = 1;        //funnel and sensor
//    }
    sideButton1Data = digitalRead(sideButton1);
    sideButton2Data = digitalRead(sideButton2);

    //HOLE IS DETECTED
//     Serial.println("Sidebutton1: " + String(sideButton1Data) + " SideButton2: " + String(sideButton2Data));
    Serial.println("Val: " + String(val) + " Old Val: " + String(old_val));
    if((val>old_val+1200) && (sideButton1Data == 1 && sideButton2Data == 1)){
      int counterTime = 0;
      Serial.println("in first loop");
      //POST HOLE DETECTED FOR 10 SECONDS
      while(counterTime < 7000 ){ //allows time for reading message posted on server
        post("HOLE DETECTED"); 
//        Serial.println("in while loop");
        delay(1);
        counterTime++;
      }
      
      //transitions to get request to move servo
//      while(state == 1){ //FOR BUTTONS WITH YELLOW TAPE
      while(state == 0){ //FOR BUTTONS WITHOUT YELLOW TAPE
          state = digitalRead(buttonPin);
          getData();
      }
      setup2();
    }
    else{
      post("No value");
    }

    
    if (counter % 20 == 0){ //wait 20 data points before changing old value
      old_val=analogRead(sensorpin); //gives time for the data to change significantly
      counter++;
    }
}
