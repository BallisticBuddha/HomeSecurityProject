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

//Prepare SD card (eth shield uses pin 4)
#define chipSelect 4

//Prepare the camera
SoftwareSerial cameraconnection = SoftwareSerial(1, 0);
Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

//Digital Pin Assignments
const int rPin = 3;
const int gPin = 5;
const int bPin = 6;
const int s5Pin = 2;

//Analog Pin Assignments
const int s1Pin = 0;
const int s2Pin = 1;
const int s3Pin = 2;
const int s4Pin = 3;

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
} */

int sendEvent(Event e){
  //TODO: Send the event object to the server.
  //Serial.println(toBinString(e.getBytes(), e.getEventSize()));
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

char appendPasscodeString(char c){
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

char* snapPicture(){
  //Serial.println("Snap in 3 secs...");
  delay(3000);

  if (! cam.takePicture()) 
    Serial.println("Failed to snap!");
  else 
    Serial.println("Picture taken!");
  
  // Create an image with the name IMAGExx.JPG
  char filename[13];
  strcpy(filename, "IMAGE00.JPG");
  for (int i = 0; i < 100; i++) {
    filename[5] = '0' + i/10;
    filename[6] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }
  
  // Open the file for writing
  File imgFile = SD.open(filename, FILE_WRITE);

  // Get the size of the image (frame) taken  
  uint16_t jpglen = cam.frameLength();
  Serial.print("Storing ");
  Serial.print(jpglen, DEC);
  Serial.print(" byte image.");

  int32_t time = millis();
  pinMode(8, OUTPUT);
  // Read all the data up to # bytes!
  byte wCount = 0; // For counting # of writes
  while (jpglen > 0) {
    // read 32 bytes at a time;
    uint8_t *buffer;
    uint8_t bytesToRead = min(32, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
    buffer = cam.readPicture(bytesToRead);
    imgFile.write(buffer, bytesToRead);
    if(++wCount >= 64) { // Every 2K, give a little feedback so it doesn't appear locked up
      Serial.print('.');
      wCount = 0;
    }
    //Serial.print("Read ");  Serial.print(bytesToRead, DEC); Serial.println(" bytes");
    jpglen -= bytesToRead;
  }
  imgFile.close();

  time = millis() - time;
  Serial.println("done!");
  Serial.print(time); Serial.println(" ms elapsed");

  return filename;
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
  bool triggered[8] = {analogRead(s1Pin), analogRead(s2Pin), analogRead(s3Pin), analogRead(s4Pin), digitalRead(s5Pin), 0, 0 ,0};

  switch(devState){
    case ALARMING:
      if (key == 'D'){
        devState = WAITING_FOR_DISARM;
        usernameInput = "";
        passcodeInput = "";
        deviceEvent = new Event(DISARM);
        Event de = *deviceEvent;
        de.setPicture(snapPicture());
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

