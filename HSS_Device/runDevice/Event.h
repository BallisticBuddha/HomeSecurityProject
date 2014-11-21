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
    int sensorIDs[5];
    String picturePath;
  
  public:
    Event(EventType type);
    String typeString();
    void setType(EventType et);
    void setUser(int uid);
    void setSensors(int sensors[5]);
    void setPicture(String pth);
    ArduinoJson::Generator::JsonObject<5> getAsJson();

};
