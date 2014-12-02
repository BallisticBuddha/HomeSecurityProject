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
    unsigned int userID;
    byte* sensorIDs;
    int picSize;
    unsigned int eventSize;
    byte* data;
  
  public:
    Event(EventType type, long seq);
    byte typeNumber();
    void setType(EventType et);
    void setUser(unsigned int uid);
    void setSensors(bool sensors[8]);
    void setPicture(int pSize);
    unsigned int getEventSize();
    byte* getBytes();
    void freeData();

};
