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
String passcode = "";

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
    case DISARMED:
      if (key == 'A'){
        devState = WAITING_FOR_INPUT;
        previouslyArmed = 0;
      }
      break;
    case WAITING_FOR_INPUT:
      if (waitCycleCount >= cyclesToAlarm){
        if (previouslyArmed){
          devState = ALARMING;
        }
        else{
          devState = DISARMED; 
        }
        
        waitCycleCount = 0;
      }
      
      if (isdigit(key)){
        keypadInput += key;
      }
      else if (key == '#'){
        if (auth.authenticate(keypadInput)){
          if (previouslyArmed){
            devState = DISARMED;
          }
          else{
            devState = ARMED; 
          }
        }
        keypadInput = "";
      }
      
      break;
    
  }

  Serial.println(keypadInput);

  RGBColor color = getStateColor(devState);
  analogWrite(rPin, color.red);
  analogWrite(gPin, color.green);
  analogWrite(bPin, color.blue);

  delay(cycleTime);
  if (devState == WAITING_FOR_INPUT){
    waitCycleCount += 1;
  }
}

