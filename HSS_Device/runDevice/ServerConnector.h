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
    int sPort;
    int connect();
  
  public:
    ServerConnector(EthernetClient eClient, byte *server, int port);
    
    int authenticate(User u);
    int sendEvent(byte *arr, int len);
      
};
