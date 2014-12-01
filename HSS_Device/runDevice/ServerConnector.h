#include <WString.h>
#include <Ethernet.h>
#include <Adafruit_VC0706.h>

struct User{
  String userID;
  String passcode;
};

class ServerConnector{
  
  private:
    EthernetClient ethClient;
    int MSS;
    int ackDelayCycle;
    int ackTimeout;
    byte sAddr[4];
    int aPort;
    int ePort;
    int connectToAuth(bool quiet);
    int connectToEvent(bool quiet);
    bool serverAlive(byte *hb);
  
  public:
    ServerConnector(EthernetClient eClient, byte *server, int aPort, int ePort);
    int authenticate(User u);
    bool sendEvent(byte *arr, int len, Adafruit_VC0706 cam);
    bool sendHeartbeat();
      
};