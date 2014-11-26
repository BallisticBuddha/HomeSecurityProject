#include "ServerConnector.h"

ServerConnector::ServerConnector(EthernetClient eClient, byte *server, int aPort, int ePort){
  ethClient = eClient;
  for (int i=0; i<4; i++){
    sAddr[i] = server[i];
  }
  this->aPort = aPort;
  this->ePort = ePort;
  authorizedUser.userID = "5";
  authorizedUser.passcode = "1234";
}

int ServerConnector::connectToAuth(){
  int result = ethClient.connect(sAddr, aPort);
  if (result)
    Serial.println("Connected to authentication server.");

  return result;
}

int ServerConnector::connectToEvent(){
  int result = ethClient.connect(sAddr, ePort);
  if (result)
    Serial.println("Connected to event consumer server.");

  return result;
}

int ServerConnector::authenticate(User u){
  int res = connectToAuth();

  if (!res){
    return res;
  }

  if (sizeof(u.userID) > 2){
    for (int i=0; i < 2; i++){
      u.userID = u.userID.substring(0,2);
    }
  }

  String toSend = "uid:";
  toSend += u.userID;
  toSend += ",pass:";
  toSend += u.passcode;

  ethClient.println(toSend);

  String recvMsg = "";
  while (ethClient.connected()){
    if (ethClient.available()){
      char cRecv = ethClient.read();
      if (cRecv >= 0){
        recvMsg += cRecv;
      }
    }
  }

  ethClient.stop();
  ethClient.flush();
  Serial.println("Disconnected from authentication server.");

  if (recvMsg == "success"){
    return 1;
  }
  else{
    return 0;
  }
}

int ServerConnector::sendEvent(byte *arr, int len){
  int res = connectToEvent();

  if (!res){
    return res;
  }

  ethClient.write(arr, len);

  ethClient.stop();
  ethClient.flush();
  Serial.println("Disconnected from event consumer server.");
  
  return 1;

}
