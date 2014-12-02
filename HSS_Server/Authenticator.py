#!/usr/bin/python3

import re
import errno
from socket import error as sock_error

from Server import Server
from PSQL import PSQLConn

class Authenticator(Server):

    def __init__(self, sAddr='127.0.0.1', sPort=8088, buffSize=1024):
        Server.__init__(self, sAddr, sPort, buffSize)
        self.running = True
        self.psql = PSQLConn()
        print("Authentication Server started...")


    def getAuthResponse(self, success, token):
        resPkt = []

        resPkt.append(0) # Auth Packet
        resPkt[0] = resPkt[0] | 2 << 4 # Response
        if success:
            resPkt[0] = resPkt[0] | 1
        resPkt.append(token >> 16)
        resPkt.append((token >> 8) & 0xFF)
        resPkt.append(token & 0xFF)

        return bytes(resPkt)


    def listenForAuth(self):
        while self.running:
            self.sock.listen(0)
            cSock, cAddr = self.sock.accept()
            print("[Authenticator] Accepted connection from %s." % str(cAddr))
            connAlive = True
            authPkt = []

            try:
                while connAlive:
                    data = cSock.recv(self.buffSize)

                    authPkt.extend(data)

                    if (authPkt[0] >> 6) == 0: # Authentication packet
                        if (authPkt[0] >> 4) & 0x30 == 1: # Auth request
                            if len(authPkt >= 8):
                                connAlive = False
                        else:
                            connAlive = False
                    else:
                        connAlive = False

                pktType = authPkt[0] >> 6
                direction = (authPkt[0] >> 4) & 0x0F
                message = authPkt[0] & 0x0F

                if pktType == 0 and len(authPkt) >= 8:
                    token = authPkt[1] << 16
                    token = token | (authPkt[2] << 8)
                    token = token | authPkt[3]

                    if direction == 1: 
                        user = authPkt[4] << 8
                        user = user | authPkt[5]

                        passcode = authPkt[6] << 8
                        passcode = passcode | authPkt[7]

                        result = self.authorized(user, passcode)
                        response = self.getAuthResponse(result, token)
                        cSock.send(response)
                        if result: 
                           print("[Authenticator] %s was successfully authenticated." % self.getUsername(result))
                        else:
                            print("[Authenticator] Failed to authenticate user with ID %s." % user)
                    else:
                        print("[Authenticator] Recieved response packet.")
                else:
                    print("[Authenticator] message too small or of wrong type.")


                cSock.close()
            except sock_error as err:
                if err.errno != errno.ECONNRESET:
                    print("[Event Consumer] An unexpected socket error occured.")
                    print(err)
                else:
                    print("[Authenticator] Connection was reset, resuming to allow new connections.")
                    print("[Authenticator] Message received before reset: %s" % message)


    def authorized(self, userID, passcode):
        ret = None

        if userID > 0:
            with self.psql as cursor:
                cursor.execute("SELECT user_id, user_pin FROM ac3app_userprofile WHERE (user_id = %s)", (userID,))
                resTup = cursor.fetchone()

                if resTup and resTup[1] == passcode:
                    ret = resTup[0]

        return ret


    def getUsername(self, userID):
        ret = None

        userID = userID
        with self.psql as cursor:
            cursor.execute("SELECT username FROM auth_user WHERE (id = %s)", (userID,))
            resTup = cursor.fetchone()

            if resTup and resTup[0]:
                ret = resTup[0]

        return ret


if __name__ == "__main__":
    auth = Authenticator(sAddr="192.168.1.49")

    auth.listenForAuth()