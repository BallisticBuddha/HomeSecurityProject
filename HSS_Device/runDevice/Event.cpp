#include "Event.h"
#include <Time.h>

Event::Event(EventType type){
  eventType = type;
  timeTriggered = now();
}

String Event::typeString(){
  String ret;
  switch(eventType){
    case DISARM:
      ret = "DISARM";
      break;
    case ARM:
      ret = "ARM";
      break;
    case ALARM:
      ret = "ALARM";
      break;
    default:
      ret = "UNKNOWN_EVENT_TYPE";
      break;
  }
  return ret;
}

void Event::setType(EventType et){
  eventType = et;
}

void Event::setUser(int uid){
  userID = uid;
}

void Event::setSensor(int sid){
  sensorID = sid;
}

void Event::setPicture(String pth){
  picturePath = pth;
}

ArduinoJson::Generator::JsonObject<5> Event::getAsJson(){
  ArduinoJson::Generator::JsonObject<5> jo;
  jo["type"] = typeString();
  jo["time"] = timeTriggered;
  jo["userID"] = userID;
  jo["sensorID"] = sensorID;
  jo["picture"] = picturePath;
  
  return jo;
}
