#include <Arduino.h>
#include <WString.h>
#include <JsonGenerator.h>

enum EventType{
  DISARM,
  ARM,
  ALARM
};

class Event{
  
  private:
    EventType eventType;
    long timeTriggered;
    byte userID[2];
    byte sensorIDs;
    String picturePath;
  
  public:
    Event(EventType type);
    byte typeNumber();
    void setType(EventType et);
    void setUser(String userID);
    void setSensors(byte sensors[8]);
    void setPicture(String pth);
    byte *getBytes();

};
