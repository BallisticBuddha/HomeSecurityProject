#include "Event.h"
#include <Time.h>

Event::Event(EventType type){
  eventType = type;
  timeTriggered = now();
  userID[0] = 0;
  userID[1] = 0;
  sensorIDs = 0;
  picturePath = "";
}

void Event::setType(EventType et){
  eventType = et;
}

void Event::setUser(String uid){

  if (uid.length() >= 2){
    userID[0] = uid[0];
    userID[1] = uid[1];
  }
  else if (uid.length() == 1){
    userID[0] = '0';
    userID[1] = uid[0];
  }
  else{
    userID[0] = 0;
    userID[1] = 0; 
  }
}

void Event::setSensors(byte sensors[8]){
  for (int i = 0; i < 8; i++){
    byte sByte;
    switch(i){
      case 0:
        sensorIDs = sensors[0];
        sensorIDs << 7;
        break;
      case 1:
        sByte = sensors[1];
        sByte << 6;
        sensorIDs = sensorIDs | sByte;
        break;
      case 2:
        sByte = sensors[2];
        sByte << 5;
        sensorIDs = sensorIDs | sByte;
        break;
      case 3:
        sByte = sensors[3];
        sByte << 4;
        sensorIDs = sensorIDs | sByte;
        break;
      case 4:
        sByte = sensors[4];
        sByte << 3;
        sensorIDs = sensorIDs | sByte;
        break;
      case 5:
        sByte = sensors[5];
        sByte << 2;
        sensorIDs = sensorIDs | sByte;
        break;
      case 6:
        sByte = sensors[6];
        sByte << 1;
        sensorIDs = sensorIDs | sByte;
        break;
      case 7:
        sByte = sensors[7];
        sensorIDs = sensorIDs | sByte;
        break;
    }
  }
}

void Event::setPicture(String pth){
  picturePath = pth;
}

byte Event::typeNumber(){
  byte ret;
  switch(eventType){
    case ARM:
      ret = 1;
      break;
    case DISARM:
      ret = 2;
      break;
    case ALARM:
      ret = 3;
      break;
    default:
      ret = 0;
      break;
  }
  return ret;
}


byte *Event::getBytes(){
  byte toRet[4];

  // first 2 bits are packet type, 2 for event
  toRet[0] = 2;
  toRet[0] << 6;

  Serial.println(toRet[0]);

  // next 2 are event type
  byte eType = Event::typeNumber();
  eType << 4;
  toRet[0] = toRet[0] | eType;

  Serial.println(toRet[0]);

  // next 4 are for the picture type (0 for no picture)
  byte pType;
  if (picturePath.length() == 0){
      pType = 0;
  }
  else{
    pType = 1;
  }
  toRet[0] = toRet[0] | pType;

  Serial.println(toRet[0]);

  // next 2 bytes represent the userID
  toRet[1] = userID[0];
  toRet[2] = userID[1];

  // next byte is for the sensor IDs
  toRet[3] = sensorIDs;

  // next 4 bytes are for the time triggered
  

  return toRet;
}


