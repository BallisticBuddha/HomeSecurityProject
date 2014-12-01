#include "ServerConnector.h"

ServerConnector::ServerConnector(EthernetClient eClient, byte *server, int aPort, int ePort){
  ethClient = eClient;
  for (int i=0; i<4; i++){
    sAddr[i] = server[i];
  }
  this->aPort = aPort;
  this->ePort = ePort;
  MSS = 1460;
  ackDelayCycle = 100;
  ackTimeout = 300; // 30 seconds
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
  bool ret = false;

  if (packetType == 1){ // heartbeat packet
    int hbType = (hb[0] & 0x30) >> 4;
    if (hbType == 2){ // response packet
      int resMsg = hb[0] & 0x0F;
      if (resMsg == 1) // 1 means the server is alive
        ret = true;
      else if (resMsg == 0) // 0 means the server has been set to no longer run
        Serial.println("Server is responding as disabled.");
      else{
        Serial.print("Received an invalid server status of ");
        Serial.println(resMsg);
      }
    }
    else{
      Serial.print("Invalid heartbeat type (");
      Serial.print(hbType);
      Serial.println(") received for heartbeat.");
    }
  }
  else{
    Serial.print("Invalid packet type (");
    Serial.print(packetType);
    Serial.println(") received for heartbeat.");
  }

  return ret;
}

int ServerConnector::authenticate(User u){
  int res = connectToAuth();

  if (!res){
    return res;
  }

  if (sizeof(u.userID) > 2){
    u.userID = u.userID.substring(0,2);
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

bool ServerConnector::sendEvent(byte* arr, int len, Adafruit_VC0706 cam){
  int res = connectToEvent();

  if (!res){
    return res;
  }

  long seqNum = arr[4] << 24;
  seqNum = (arr[5] << 16) | seqNum;
  seqNum = (arr[6] << 8) | seqNum;
  seqNum = arr[7] | seqNum;

  ethClient.write(arr, len);

  // The rest of the data contains the picture
  if (len == 12){
    int picSize = arr[8] << 24;
    picSize = (arr[9] << 16) | picSize;
    picSize = (arr[10] << 8) | picSize;
    picSize = arr[11] | picSize;

    int camBuffSize = 64;
    int bytesLeft = picSize;
    while (bytesLeft > 0){
      int toSend = min(MSS, bytesLeft);
      int bytesLeftInChunk = toSend;
      byte* sendBuffer = new byte[toSend];
      int offset = 0;
      while (bytesLeftInChunk > 0){
        int toRead = min(camBuffSize, bytesLeft);
        memcpy(sendBuffer + offset, cam.readPicture(toRead), toRead);
        offset += toRead;
        bytesLeftInChunk -= toRead;
      }
      ethClient.write(sendBuffer, toSend);
      delete sendBuffer;
      bytesLeft -= toSend;
    }
  }

  // Wait for an ACK so we don't close the connection too early
  unsigned int cycleCount = 0;
  while (!ethClient.available()){
    delay(ackDelayCycle);
    cycleCount++;

    if (cycleCount >= ackTimeout){
      break;
    }
  }

  Serial.println(cycleCount);

  bool acked = false;
  byte* eventACK = new byte;
  byte* iterPtr = eventACK;
  unsigned int recvLen = 0;
  while(ethClient.connected()){
    if(ethClient.available()){
      *iterPtr = ethClient.read();
      iterPtr++;
      recvLen++;
    }
  }

  // The first 2 bits are the packet type (3 for event ACK)
  int pType = eventACK[0] >> 6;
  // The next 2 bits represent the type of event that was acked
  int eType = (eventACK[0] & 0x30) >> 4;

  // The next 28 bits are padding

  // The next 4 bytes are the sequence number that this is acking
  long ackNum = eventACK[4] << 24;
  ackNum = (eventACK[5] << 16) | ackNum;
  ackNum = (eventACK[6] << 8) | ackNum;
  ackNum = eventACK[7] | ackNum;

  if (pType == 3 && eType == (arr[0] & 0x30) >> 4 && ackNum == seqNum)
    acked = true;
  else if (cycleCount >= ackTimeout){
    Serial.print("Connection timed out after ");
    Serial.print((ackTimeout * ackDelayCycle) / 1000);
    Serial.println(" seconds.");
  }
  else{
    Serial.println("Event was not acked before the server closed the connection.");
    Serial.println("Bytes received:");
    for (int i=0; i < recvLen; i++){
      Serial.print(eventACK[i], BIN);
      Serial.print(" ");
    }
    Serial.println("");
  }

  ethClient.stop();
  ethClient.flush();
  Serial.println("Disconnected from event consumer server.");
  
  return acked;

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
