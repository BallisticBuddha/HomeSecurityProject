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
    byte userID[2];
    byte* sensorIDs;
    String picturePath;
    unsigned int eventSize;
    byte* data;
  
  public:
    Event(EventType type);
    byte typeNumber();
    void setType(EventType et);
    void setUser(String userID);
    void setSensors(bool sensors[8]);
    void setPicture(String pth);
    unsigned int getEventSize();
    byte *getBytes();
    void freeData();

};
