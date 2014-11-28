#include <Arduino.h>
#include <Wire.h>
#include "SPI.h"
#include <Ethernet.h>
#include <Keypad_I2C.h>
#include <Keypad.h>
#include <Adafruit_VC0706.h>
#include <SD.h>
#include <SoftwareSerial.h> 

#include "HSSDevice.h"
#include "ServerConnector.h"
#include "Event.h"

//Prepare SD card
#define chipSelect 4

// Prepare Camera
// Using hardware serial on Mega: camera TX conn. to RX1,
// camera RX to TX1, no SoftwareSerial object is required:
Adafruit_VC0706 cam = Adafruit_VC0706(&Serial1);

//Prepare I/O Expander for keypad
byte rowPins[ROWS] = {0,1,2,3};
byte colPins[COLS] = {4,5,6,7};
int i2caddress = 0x20;

//Prepare for network connectivity 
byte MAC[] = {0x1C , 0x02, 0x75, 0xBD, 0xDC, 0x44};
byte IP[] = {192, 168, 1, 48};
byte server[] = {192, 168, 1, 106};
int authPort = 8088;
int eventPort = 8089;

//Analog pin assignments
const int rPin = 3;
const int gPin = 5;
const int bPin = 6;

//Digital pin assignments
const int s1Pin = 22;
const int s2Pin = 23;
const int s3Pin = 24;
const int s4Pin = 25;
const int s5Pin = 26;
const int s6Pin = 27;
const int s7Pin = 28;
const int s8Pin = 29;

//Device Variables
const int cycleTime = 10; // 10 miliseconds
const int cyclesPerHeartbeat = 500; // 5 seconds
const int cyclesToAlarm = 1500; // 15 seconds
//const int cyclesToAlarm = 500; // 5 seconds

EthernetClient client;
ServerConnector sConn(client, server, authPort, eventPort);
Keypad_I2C keypad = Keypad_I2C( makeKeymap(keys), rowPins, colPins, ROWS, COLS, i2caddress );
DeviceState devState = DISARMED;
DeviceState prevState = devState;
unsigned int waitCycleCount = 0;
unsigned int hbCycleCount = 0;

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
  sConn.sendEvent(e.getBytes(), e.getEventSize());
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
  // When using hardware SPI, the SS pin MUST be set to an
  // output (even if not connected or used).  If left as a
  // floating input w/SPI on, this can cause lockuppage.
#if !defined(SOFTWARE_SPI)
  if(chipSelect != 53) pinMode(53, OUTPUT); // SS on Mega
#endif

  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(s1Pin, INPUT);
  pinMode(s2Pin, INPUT);
  pinMode(s3Pin, INPUT);
  pinMode(s4Pin, INPUT);
  pinMode(s5Pin, INPUT);
  pinMode(s6Pin, INPUT);
  pinMode(s7Pin, INPUT);
  pinMode(s8Pin, INPUT);

  Serial.begin(9600);
  Ethernet.begin(MAC, IP);
  keypad.begin();

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)){
    Serial.println("SD card failed, or not present");
    return;
  } 

  // Try to locate the camera
  if (cam.begin()) {
    Serial.println("Camera Found:");
  }
  else {
    Serial.println("No camera found?");
    return;
  }
  // Print out the camera version information (optional)
  char *reply = cam.getVersion();
  if (reply == 0) {
    Serial.print("Failed to get version");
  } else {
    Serial.println("-----------------");
    Serial.print(reply);
    Serial.println("-----------------");
  }

  cam.setImageSize(VC0706_640x480);        // biggest
  //cam.setImageSize(VC0706_320x240);        // medium
  //cam.setImageSize(VC0706_160x120);          // small
}

void loop(){
  if ((hbCycleCount % cyclesPerHeartbeat) == 0){
    if (!sConn.sendHeartbeat()){
      if (devState != DISABLED){
        Serial.println("Server is unavailable or not responding, disabling device.");
        prevState = devState;
        devState = DISABLED;
      }
    }
    else if(devState == DISABLED){
      Serial.println("Server is responding again, enabling device.");
      devState = prevState;
    }
  }

  char key = keypad.getKey();
  bool triggered[8] = {digitalRead(s1Pin), digitalRead(s2Pin), digitalRead(s3Pin), 
    digitalRead(s4Pin), digitalRead(s5Pin), digitalRead(s6Pin), digitalRead(s7Pin), digitalRead(s8Pin)};

  switch(devState){
    case ALARMING:
      if (key == 'D'){
        //TODO: Take a picture here
        devState = WAITING_FOR_DISARM;
        waitCycleCount = 0;
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
            if (sConn.authenticate(deviceUser)){
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
            if (sConn.authenticate(deviceUser)){
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
  hbCycleCount++;
  if (devState == WAITING_FOR_ARM || devState == WAITING_FOR_DISARM || devState == WAITING_TO_ARM || devState == ALARMING){
    lightUp(1);
    waitCycleCount++;
  }
  else{
    lightUp(0);
  }
}

