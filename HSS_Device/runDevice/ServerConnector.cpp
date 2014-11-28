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

int ServerConnector::connectToAuth(bool quiet = false){
  int result = ethClient.connect(sAddr, aPort);
  String conMsg;
  if (result)
    conMsg = "Connected to authentication server.";
  else
    conMsg = "Unable to connect to authentication server.";
  
  if (!quiet)
    Serial.println(conMsg);

  return result;
}

int ServerConnector::connectToEvent(bool quiet = false){
  int result = ethClient.connect(sAddr, ePort);
  String conMsg;
  if (result)
    conMsg = "Connected to event consumer server.";
  else
    conMsg = "Unable to connect to event consumer server.";
  
  if (!quiet)
    Serial.println(conMsg);

  return result;
}


bool ServerConnector::serverAlive(byte *hb){
  int packetType = (hb[0] & 0xC0) >> 6;

  if (packetType == 1){ // heartbeat packet
    int hbType = (hb[0] & 0x30) >> 4;
    if (hbType == 2){ // response packet
      int resMsg = hb[0] & 0x0F;
      if (resMsg == 1) // 1 means the server is alive
        return true;
      else if (resMsg == 0) // 0 means the server has been set to no longer run
        return false;
    }
    else
      Serial.println("Received non-response packet from server.");
  }
  else
    Serial.println("Invalid packet type for heartbeat received.");
  return false;
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

bool ServerConnector::sendHeartbeat(){
  int res = connectToEvent(true);

  if (!res){
    return res;
  }

  byte* hbRequest = new byte;

  // The first 2 bits are the packet type (1 for heartbeats)
  *hbRequest = 1 << 6;

  // The next 2 bits indicate the HB message type (1 for request, 2 for response)
  *hbRequest = *hbRequest | (1 << 4);

  // The last 4 bits are reserved for the response message from the server, so we'll leave them zeroed on the request

  ethClient.write(hbRequest, 1);

  delete hbRequest;
  hbRequest = NULL;

  byte* hbResponse = new byte;
  byte* iterPtr = hbResponse;
  unsigned int recvLen = 0;
  while(ethClient.connected()){
    if(ethClient.available()){
      *iterPtr = ethClient.read();
      iterPtr++;
      recvLen++;
    }
  }

  ethClient.stop();
  ethClient.flush();

  if (recvLen > 1)
    Serial.println("More than 1 byte received for heartbeat, this shouldn't happen.");

  bool alive = serverAlive(hbResponse);

  delete hbResponse;
  hbResponse = NULL;
  iterPtr = NULL;

  return alive;
}
