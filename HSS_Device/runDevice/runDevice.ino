#include <Keypad.h>
#include "HSSDevice.h"
#include "Authenticator.h"

//Pin Assignments
byte rowPins[ROWS] = {12,8,7,6}; //connect to row pinouts 
byte colPins[COLS] = {5,4,3,2}; //connect to column pinouts
const int rPin = 9;
const int gPin = 10;
const int bPin = 11;
const int sensorPin = 13;

//Device Variables
const int cycleTime = 10; // 10 miliseconds
int cyclesToAlarm = 1500; // 15 seconds

Authenticator auth;
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
DeviceState devState = DISARMED;
int previouslyArmed = 0;
int waitCycleCount = 0;

String keypadInput = "";

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
    case WAITING_FOR_INPUT:
      fullColor = wfiColor;
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

void lightUp(int flashing){
  RGBColor color = getStateColor(devState);

  if (flashing){ // fading light
    if ((waitCycleCount % 512) < 256){ // fade up
      analogWrite(rPin, color.red * ((waitCycleCount % 256) / 255.0) );
      analogWrite(rPin, color.green * ((waitCycleCount % 256) / 255.0) );
      analogWrite(rPin, color.blue * ((waitCycleCount % 256) / 255.0) );
    }
    else{ // fade down
      analogWrite(rPin, color.red - (512 - (waitCycleCount % 512)) );
      analogWrite(rPin, color.green - (512 - (waitCycleCount % 512)) );
      analogWrite(rPin, color.blue - (512 - (waitCycleCount % 512)) );
    }

  }
  else{ // solid light
    analogWrite(rPin, color.red);
    analogWrite(gPin, color.green);
    analogWrite(bPin, color.blue);
  }

}

void setup(){
  Serial.begin(9600);

  pinMode(rPin, OUTPUT);
  pinMode(rPin, OUTPUT);
  pinMode(rPin, OUTPUT);
  pinMode(sensorPin, INPUT);

}

void loop(){
  char key = keypad.getKey();
  int sensorState = digitalRead(sensorPin);

  switch(devState){
    case ALARMING:
      if (key == 'D'){
        devState = WAITING_FOR_INPUT;
        previouslyArmed = 1;
        keypadInput = "";
      }
    case ARMED:
      if (sensorState || key == 'D'){
        //TODO: Take a picture here
        devState = WAITING_FOR_INPUT;
        previouslyArmed = 1;
        keypadInput = "";
      }
    case WAITING_TO_ARM:
      if (waitCycleCount >= cyclesToAlarm){
        devState = ARMED;
        waitCycleCount = 0;
      }
      break;
    case DISARMED:
      if (key == 'A'){
        devState = WAITING_FOR_INPUT;
        previouslyArmed = 0;
        keypadInput = "";
      }
      break;
    case WAITING_FOR_INPUT:
      // Change state if time has expired
      if (waitCycleCount >= cyclesToAlarm){
        if (previouslyArmed){
          devState = ALARMING;
        }
        else{
          devState = DISARMED; 
        }
        waitCycleCount = 0;
      }
      // Accept keypad input
      if (isdigit(key)){
        keypadInput += key;
      }
      else if (key == '#'){
        if (auth.authenticate(keypadInput)){
          if (previouslyArmed){
            devState = DISARMED;
          }
          else{
            devState = WAITING_TO_ARM; 
          }
        }
        keypadInput = "";
      }
      else if (key == 'D'){
        keypadInput = "";
      }
      break;
    
  }

  delay(cycleTime);
  if (devState == WAITING_FOR_INPUT || devState == WAITING_TO_ARM || devState == ALARMING){
    lightup(1)
    waitCycleCount += 1;
  }
  else{
    lightUp(0)
  }
}

