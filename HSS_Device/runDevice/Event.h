#include <WString.h>
#include <JsonGenerator.h>

enum EventType{
  DISARM,
  ARM,
  ALARM,
  CORRECT_PASSCODE,
  INCORRECT_PASSCODE
};

class Event{
  
  private:
    int eventID;
    EventType eventType;
    long timeTriggered;
    int userID;
    int sensorID;
    String picturePath;
  
  public:
    Event(int ID, EventType type);
    String typeString();
    void setUser(int uid);
    void setSensor(int sid);
    void setPicture(String path);
    ArduinoJson::Generator::JsonObject<6> getAsJson();

};
