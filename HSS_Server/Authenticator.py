#!/usr/bin/python3

import re
from Server import Server
from PSQL import PSQLConn

class Authenticator(Server):

    def __init__(self, sAddr='127.0.0.1', sPort=8088, buffSize=1024):
        Server.__init__(self, sAddr, sPort, buffSize)
        self.running = True
        self.psql = PSQLConn()
        self.msgre = re.compile("uid:([0-9]*),pass:([0-9]*)")
        print("Authentication Server started...")


    def listenForAuth(self):
        while self.running:
            self.sock.listen(0)
            cSock, cAddr = self.sock.accept()
            print("[Authenticator] Accepted connection from %s." % str(cAddr))
            connAlive = True
            message = ""

            try:
                while connAlive:
                    data = cSock.recv(self.buffSize)

                    message += data.decode()

                    if data.decode() == '\n' or len(data) == 0:
                        connAlive = False

                reRes = self.msgre.search(message)
                if not reRes or len(reRes.groups()) < 2:
                    print("[Authenticator] malformed authentication request received: \n%s" % message)
                else:
                    userID = authorized(reRes.group(1), reRes.group(2))
                    if userID:
                        print("[Authenticator] %s was successfully authenticated." % getUsername(userID))
                        cSock.send("success".encode())
                    else:
                        print("[Authenticator] Failed to authenticate user with ID %s." % reRes.group(1))
                        cSock.send("fail".encode())

                cSock.close()
            except ConnectionResetError as e:
                print("[Authenticator] Connection was reset, resuming to allow new connections.")
                print("[Authenticator] Message received before reset: %s" % message)


    def authorized(self, userID, passcode):
        ret = None

        try:
            userID = int(userID)
            passcode = int(passcode)
            with self.psql as cursor:
                cursor.execute("SELECT id, user_pin FROM ac3app_userprofile WHERE (user_id = %s)" % userID)
                resTup = cursor.fetchone()

                if resTup and resTup[1] == passcode:
                    ret = resTup[0]
        except ValueError as e:
            print("[Authenticator] The received userID or passcode was not an integer.")

        return ret


    def getUsername(self, userID):
        ret = None

        try:
            userID = int(userID)
            with self.psql as cursor:
                cursor.execute("SELECT username FROM auth_user WHERE (id = %s)" % userID)
                resTup = cursor.fetchone()

                if resTup and resTup[0]:
                    ret = resTup[0]

        except ValueError as e:
            print("[Authenticator] Cannot get username of non-integer userID.")

        return ret


if __name__ == "__main__":
    auth = Authenticator(sAddr="192.168.1.49")

    auth.listenForAuth()