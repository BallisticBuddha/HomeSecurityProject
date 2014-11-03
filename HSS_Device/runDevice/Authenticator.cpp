#include "Authenticator.h"

Authenticator::Authenticator(){
  correctCode = "1234";
}

int Authenticator::authenticate(String passcode){
  if (correctCode == passcode){
    return 1;
  }
  else{
    return 0; 
  }
}
