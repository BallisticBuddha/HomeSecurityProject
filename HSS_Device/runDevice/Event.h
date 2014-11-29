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
    long seqNum;
    byte userID[2];
    byte* sensorIDs;
    int picSize;
    unsigned int eventSize;
    byte* data;
  
  public:
    Event(EventType type);
    byte typeNumber();
    void setType(EventType et);
    void setUser(String userID);
    void setSensors(bool sensors[8]);
    void setPicture(int pSize);
    unsigned int getEventSize();
    byte* getBytes();
    void freeData();

};
