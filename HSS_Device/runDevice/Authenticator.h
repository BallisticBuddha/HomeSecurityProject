#include <WString.h>

class Authenticator{
  
  private:
    String correctCode;
  
  public:
    Authenticator();
    
    int authenticate(String passcode);
      
};
