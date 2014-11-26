#include "ServerConnector.h"

ServerConnector::ServerConnector(EthernetClient eClient, byte *server, int port){
  ethClient = eClient;
  for (int i=0; i<4; i++){
    sAddr[i] = server[i];
  }
  sPort = port;
  authorizedUser.userID = "5";
  authorizedUser.passcode = "1234";
}

int ServerConnector::connect(){
  int result = ethClient.connect(sAddr, sPort);
  if (result)
    Serial.println("Connected to server");

  return result;
}

int ServerConnector::authenticate(User u){
  int res = this->connect();

  if (!res){
    return res;
  }

  String toSend = "uid:";
  toSend += u.userID;
  toSend += ",pass:";
  toSend += u.passcode;

  ethClient.println(toSend);

  

  if (authorizedUser.userID == u.userID && authorizedUser.passcode == u.passcode){
    return 1;
  }
  else{
    return 0; 
  }
}

int ServerConnector::sendEvent(byte *arr, int len){
  if (this->connect()){
     ethClient.write(arr, len);
     return 1;
  }
  
  return 0;

}
