#include <Arduino.h>
#include <WString.h>

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
    byte* picture;
    unsigned int eventSize;
    byte* data;
  
  public:
    Event(EventType type);
    byte typeNumber();
    void setType(EventType et);
    void setUser(String userID);
    void setSensors(bool sensors[8]);
    void setPicture(int picSize, byte *pic);
    unsigned int getEventSize();
    byte *getBytes();
    void freeData();

};
