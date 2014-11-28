#include <WString.h>
#include <Ethernet.h>

struct User{
  String userID;
  String passcode;
};

class ServerConnector{
  
  private:
    User authorizedUser;
    EthernetClient ethClient;
    byte sAddr[4];
    int aPort;
    int ePort;
    int connectToAuth(bool quiet);
    int connectToEvent(bool quiet);
    bool serverAlive(byte *hb);
  
  public:
    ServerConnector(EthernetClient eClient, byte *server, int aPort, int ePort);
    
    int authenticate(User u);
    int sendEvent(byte *arr, int len);
    bool sendHeartbeat();
      
};
