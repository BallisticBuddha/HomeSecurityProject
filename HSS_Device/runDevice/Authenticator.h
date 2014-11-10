#include <WString.h>

struct User{
  String userID;
  String passcode;
};

class Authenticator{
  
  private:
    User authorizedUser;
  
  public:
    Authenticator();
    
    int authenticate(User u);
      
};
