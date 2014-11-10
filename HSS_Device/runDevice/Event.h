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
    int userID;
    int sensorID;
    String picturePath;
  
  public:
    Event(EventType type);
    ~Event();
    String typeString();
    void setType(EventType et);
    void setUser(int uid);
    void setSensor(int sid);
    void setPicture(String pth);
    ArduinoJson::Generator::JsonObject<5> getAsJson();

};
