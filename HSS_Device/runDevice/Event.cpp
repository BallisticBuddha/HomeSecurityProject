#include "Event.h"
#include <Time.h>

Event::Event(int ID, EventType type){
  eventID = ID;
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
    case CORRECT_PASSCODE:
      ret = "CORRECT_PASSCODE";
      break;
    case INCORRECT_PASSCODE:
      ret = "INCORRECT_PASSCODE";
      break;
    default:
      ret = "UNKNOWN_EVENT_TYPE";
      break;
  }
  return ret;
}

void Event::setUser(int uid){
  userID = uid;
}

void Event::setSensor(int sid){
  sensorID = sid;
}

void Event::setPicture(String path){
  picturePath = path;
}

ArduinoJson::Generator::JsonObject<6> Event::getAsJson(){
  ArduinoJson::Generator::JsonObject<6> jo;
  jo["ID"] = eventID;
  jo["type"] = typeString();
  jo["time"] = timeTriggered;
  jo["userID"] = userID;
  jo["sensorID"] = sensorID;
  jo["picture"] = picturePath;
  
  return jo;
}
