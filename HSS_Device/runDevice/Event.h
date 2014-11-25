#include <Arduino.h>

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
    char* picturePath;
    unsigned int eventSize;
    byte* data;
  
  public:
    Event(EventType type);
    byte typeNumber();
    void setType(EventType et);
    void setUser(String userID);
    void setSensors(bool sensors[8]);
    void setPicture(char *pth);
    unsigned int getEventSize();
    byte *getBytes();
    void freeData();

};
