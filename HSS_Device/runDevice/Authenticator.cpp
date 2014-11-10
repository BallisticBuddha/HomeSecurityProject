#include "Authenticator.h"

Authenticator::Authenticator(){
  authorizedUser.userID = "5";
  authorizedUser.passcode = "1234";
}

int Authenticator::authenticate(User u){
  if (authorizedUser.userID == u.userID && authorizedUser.passcode == u.passcode){
    return 1;
  }
  else{
    return 0; 
  }
}
