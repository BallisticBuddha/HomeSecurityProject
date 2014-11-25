#include <Arduino.h>
#include <Wire.h>
#include "SPI.h"
#include <Ethernet.h>
#include <Keypad_I2C.h>
#include <Keypad.h>
#include <JsonGenerator.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <Adafruit_VC0706.h>
#include <SD.h>
#include <SoftwareSerial.h>   

#include "HSSDevice.h"
#include "Authenticator.h"
#include "Event.h"

//Prepare I/O Expander for keypad
byte rowPins[ROWS] = {0,1,2,3};
byte colPins[COLS] = {4,5,6,7};
int i2caddress = 0x20;

//Prepare for network connectivity 
byte MAC[] = {0x1C , 0x02, 0x75, 0xBD, 0xDC, 0x44};
byte IP[] = {192, 168, 1, 148};
byte server[] = {192, 168, 1, 106};
EthernetClient client;

//Prepare SD card
#define chipSelect 4

//Prepare the camera
SoftwareSerial cameraconnection = SoftwareSerial(1, 0);
Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);



//Pin Assignments
const int rPin = 3;
const int gPin = 5;
const int bPin = 6;

const int s1Pin = 2;
const int s2Pin = 4;
const int s3Pin = 7;
const int s4Pin = 8;
const int s5Pin = 9;

//Device Variables
const int cycleTime = 10; // 10 miliseconds
//int cyclesToAlarm = 1500; // 15 seconds
int cyclesToAlarm = 500; // 5 seconds

Authenticator auth;
Keypad_I2C keypad = Keypad_I2C( makeKeymap(keys), rowPins, colPins, ROWS, COLS, i2caddress );
DeviceState devState = DISARMED;
unsigned int waitCycleCount = 0;

User deviceUser;
Event* deviceEvent = NULL;
String usernameInput = "";
String passcodeInput = "";

RGBColor getStateColor(DeviceState ds){
  RGBColor ret;
  unsigned long fullColor;
  
  switch(ds){
    case ARMED:
      fullColor = armedColor;
      break;
    case DISARMED:
      fullColor = disarmedColor;
      break;
    case WAITING_FOR_ARM:
      fullColor = wfaColor;
      break;
    case WAITING_FOR_DISARM:
      fullColor = wfdColor;
      break;
    case WAITING_TO_ARM:
      fullColor = wtaColor;
      break;
    case ALARMING:
      fullColor = alarmingColor;
      break;
    default:
      fullColor = disabledColor;
      break;
  }
  ret.red = fullColor >> 16;
  ret.green = (fullColor & 0x00FF00) >> 8;
  ret.blue = fullColor & 0x0000FF;
  
  return ret;
}

/*
  For debugging purposes only
  Converts a byte array (event or authentication packet) into a string.
  To be used to print packets out to the serial monitor for analysis.
*/
String toBinString(byte arr[], int arrSize){
  String toRet = "";
  for (int i=0; i < arrSize; i++){
    for (int j=0; j < 8; j++){
      int val = *arr & (1 << (7 - j));
      toRet += ( val ? '1' : '0' );
    }
    toRet += " ";
    arr++;
  }
  return toRet;
}

int sendEvent(Event e){
  //TODO: Send the event object to the server.
  Serial.println(toBinString(e.getBytes(), e.getEventSize()));
  e.freeData();
  delete deviceEvent;
  deviceEvent = NULL;
  return 0;
}

void lightUp(int flashing){
  RGBColor color = getStateColor(devState);

  if (flashing){ // fading light
    if ((waitCycleCount % 512) < 256){ // fade up
      analogWrite(rPin, color.red * ((waitCycleCount % 256) / 255.0) );
      analogWrite(gPin, color.green * ((waitCycleCount % 256) / 255.0) );
      analogWrite(bPin, color.blue * ((waitCycleCount % 256) / 255.0) );
    }
    else{ // fade down
      analogWrite(rPin, 256 - color.red * (((waitCycleCount - 255) % 256) / 255.0) );
      analogWrite(gPin, 256 - color.green * (((waitCycleCount - 255) % 256) / 255.0) );
      analogWrite(bPin, 256 - color.blue * (((waitCycleCount - 255) % 256) / 255.0) );
    }
  }

  else{ // solid light
    analogWrite(rPin, color.red);
    analogWrite(gPin, color.green);
    analogWrite(bPin, color.blue);
  }
}

int appendUserString(char c){
  if (c == '*'){
    deviceUser.userID = usernameInput;
    usernameInput = "";
    return 1;
  }
  else if (isdigit(c)){
    usernameInput += c;
  }
  return 0;
}

int appendPasscodeString(char c){
  if (c == '#'){
    deviceUser.passcode = passcodeInput;
    passcodeInput = "";
    return 1;
  }
  else if (isdigit(c)){
    passcodeInput += c;
  }
  return 0;
}

void setup(){
  #if !defined(SOFTWARE_SPI)
    if(chipSelect != 10) pinMode(10, OUTPUT);
  #endif

  Serial.begin(9600);
  Ethernet.begin(MAC, IP);
  keypad.begin();

  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(s1Pin, INPUT);
  pinMode(s2Pin, INPUT);
  pinMode(s3Pin, INPUT);
  pinMode(s4Pin, INPUT);
  pinMode(s5Pin, INPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("SDCard failed, or not present");
    // don't do anything more:
    return;
  }

   // Try to locate the camera
  if (cam.begin()) {
    Serial.println("Camera Found:");
  } else {
    Serial.println("No camera found?");
    return;
  }

  // Print out the camera version information
  char *reply = cam.getVersion();
  if (reply == 0) {
    Serial.print("Failed to get version");
  } else {
    Serial.println("-----------------");
    Serial.print(reply);
    Serial.println("-----------------");
  }

  //cam.setImageSize(VC0706_640x480);        // large
  //cam.setImageSize(VC0706_320x240);        // medium
  cam.setImageSize(VC0706_160x120);          // small

}

void loop(){
  char key = keypad.getKey();
  byte triggered[8] = {digitalRead(s1Pin), digitalRead(s2Pin), digitalRead(s3Pin), digitalRead(s4Pin), digitalRead(s5Pin), 0, 0 ,0};

  switch(devState){
    case ALARMING:
      if (key == 'D'){
        //TODO: Take a picture here
        devState = WAITING_FOR_DISARM;
        usernameInput = "";
        passcodeInput = "";
        deviceEvent = new Event(DISARM);
        //TODO: set the sensor that triggered in the event object
      }
      break;
    case ARMED:
      if (triggered[0] || triggered[1] || triggered[2] || triggered[3] || triggered[4] || triggered[5] || triggered[6] || triggered[7]|| key == 'D'){
        //TODO: Take a picture here
        devState = WAITING_FOR_DISARM;
        usernameInput = "";
        passcodeInput = "";
        deviceEvent = new Event(DISARM);
        Event de = *deviceEvent;
        de.setSensors(triggered);
      }
      break;
    case WAITING_TO_ARM:
      if (waitCycleCount >= cyclesToAlarm){
        devState = ARMED;
        waitCycleCount = 0;
      }
      break;
    case DISARMED:
      if (key == 'A'){
        devState = WAITING_FOR_ARM;
        usernameInput = "";
        passcodeInput = "";
        deviceEvent = new Event(ARM);
      }
      break;
    case WAITING_FOR_ARM:
      if (key){
        if (deviceUser.userID.length() == 0){
          appendUserString(key);
        }
        else{
          if (appendPasscodeString(key)){
            if (auth.authenticate(deviceUser)){
              // Successful ARM event
              Event de = *deviceEvent;
              de.setUser(deviceUser.userID);
              sendEvent(de);
              
              devState = WAITING_TO_ARM;
              deviceUser.userID = "";
              deviceUser.passcode = "";
            }
            else{
              deviceUser.userID = "";
              deviceUser.passcode = "";
            }
          }
        }
      }
      if (waitCycleCount >= cyclesToAlarm){
        // Failed ARM event
        Event de = *deviceEvent;
        sendEvent(de);

        devState = DISARMED;
        deviceUser.userID = "";
        deviceUser.passcode = "";
        waitCycleCount = 0;     
      }
      break;
    case WAITING_FOR_DISARM:
      if (key){
        if (deviceUser.userID.length() == 0){
          appendUserString(key);
        }
        else{
          if (appendPasscodeString(key)){
            if (auth.authenticate(deviceUser)){
              // Successful DISARM event
              Event de = *deviceEvent;
              de.setUser(deviceUser.userID);
              sendEvent(de);

              devState = DISARMED;
              deviceUser.userID = "";
              deviceUser.passcode = "";
            }
            else{
              deviceUser.userID = "";
              deviceUser.passcode = "";
            }
          }
        }
      }
      
      if (waitCycleCount >= cyclesToAlarm){
        // ALARM event
        Event de = *deviceEvent;
        de.setType(ALARM);
        sendEvent(de);

        devState = ALARMING;
        deviceUser.userID = "";
        deviceUser.passcode = "";
        waitCycleCount = 0;     
      }
      break;
  }

  delay(cycleTime);
  if (devState == WAITING_FOR_ARM || devState == WAITING_FOR_DISARM || devState == WAITING_TO_ARM || devState == ALARMING){
    lightUp(1);
    waitCycleCount += 1;
  }
  else{
    lightUp(0);
  }
}

