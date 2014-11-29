#include "Event.h"

Event::Event(EventType type, long seq){
  eventType = type;
  seqNum = seq;
  userID[0] = 0;
  userID[1] = 0;
  sensorIDs = new byte;
  *sensorIDs = 0;
  picSize = 0;
  eventSize = 8;
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

void Event::setPicture(int pSize){
  Serial.print("Setting picture of length ");
  Serial.println(pSize);

  picSize = pSize;
  if (pSize > 0)
    eventSize += 4;
}

unsigned int Event::getEventSize(){
  return eventSize;
}

byte* Event::getBytes(){
  data = new byte[eventSize];
  // first 2 bits are packet type, 2 for event
  data[0] = 2;
  data[0] = data[0] << 6;

  // next 2 are event type
  byte eType = Event::typeNumber();
  eType = eType << 4;
  data[0] = data[0] | eType;

  // next 4 are for the picture type (0 for no picture, 1 for a JPEG)
  byte pType;
  if (picSize > 0)
      pType = 1;
  else
    pType = 0;
  data[0] = data[0] | pType;

  // next 2 bytes represent the userID
  data[1] = userID[0];
  data[2] = userID[1];

  // next byte is for the sensor IDs
  data[3] = *sensorIDs;

  data[4] = seqNum >> 24;
  data[5] = (seqNum >> 16) & 0xFF;
  data[6] = (seqNum >> 8) & 0xFF;
  data[7] = seqNum & 0xFF;

  // The rest of the data represents the picture taken (if there was one)
  if (picSize > 0){

    // The next 4 bytes are for the picture size
    data[8] = picSize >> 24;
    data[9] = (picSize >> 16) & 0xFF;
    data[10] = (picSize >> 8) & 0xFF;
    data[11] = picSize & 0xFF;
  }

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


