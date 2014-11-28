#include "Event.h"

Event::Event(EventType type){
  eventType = type;
  userID[0] = 0;
  userID[1] = 0;
  sensorIDs = new byte;
  picturePath = "";
  eventSize = 4;
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
    userID[0] = 0;
    userID[1] = uid[0];
  }
  else{
    userID[0] = 0;
    userID[1] = 0; 
  }
}

void Event::setSensors(bool sensors[8]){
  *sensorIDs = 0;
  for (int i = 0; i < 8; i++){
    if (sensors[i]){
      switch(i){
        case 0:
          *sensorIDs = *sensorIDs | 128;
          break;
        case 1:
          *sensorIDs = *sensorIDs | 64;
          break;
        case 2:
          *sensorIDs = *sensorIDs | 32;
          break;
        case 3:
          *sensorIDs = *sensorIDs | 16;
          break;
        case 4:
          *sensorIDs = *sensorIDs | 8;
          break;
        case 5:
          *sensorIDs = *sensorIDs | 4;
          break;
        case 6:
          *sensorIDs = *sensorIDs | 2;
          break;
        case 7:
          *sensorIDs = *sensorIDs | 1;
          break;
      }
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

unsigned int Event::getEventSize(){
  return eventSize;
}

byte *Event::getBytes(){
  data = new byte[eventSize];

  // first 2 bits are packet type, 2 for event
  data[0] = 2;
  data[0] = data[0] << 6;

  // next 2 are event type
  byte eType = Event::typeNumber();
  eType = eType << 4;
  data[0] = data[0] | eType;

  // next 4 are for the picture type (0 for no picture)
  byte pType;
  if (picturePath.length() == 0){
      pType = 0;
  }
  else{
    pType = 1;
  }
  data[0] = data[0] | pType;

  // next 2 bytes represent the userID
  data[1] = userID[0];
  data[2] = userID[1];

  // next byte is for the sensor IDs
  data[3] = *sensorIDs;

  // The rest of the data represents the picture taken (if there was one)

  return data;
}

void Event::freeData(){
  if (sensorIDs){
    delete sensorIDs;
    sensorIDs = NULL;
  }

  delete data;
  data = NULL;
}


