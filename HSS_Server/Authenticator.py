#!/usr/bin/python3

import re
from Server import Server

class Authenticator(Server):

	def __init__(self, sAddr='127.0.0.1', sPort=8088, buffSize=1024):
		Server.__init__(self, sAddr, sPort, buffSize)
		self.users = self.setUsers()
		self.running = True
		print("Authentication Server started...")
	
	def listenForAuth(self):
		while self.running:
			self.sock.listen(0)
			cSock, cAddr = self.sock.accept()
			print("Accepted connection from %s" % str(cAddr))
			connAlive = True
			message = ""

			while connAlive:
				data = cSock.recv(self.buffSize)

				message += data.decode()

				if data.decode() == '\n' or len(data) == 0:
					connAlive = False

			msgre = re.search("uid:([0-9]*),pass:([0-9]*)", message)
			if not msgre or len(msgre.groups()) < 2:
				print("malformed authentication request recieved: \n%s" % message)
			else:
				user = msgre.group(1)
				passcode = msgre.group(2)
				userTup = (user, passcode)
				if userTup in self.users:
					print("User %s successfully authenticated." % user)
					cSock.send("success".encode())
				else:
					print("Failed to authenticate user %s" % user)
					cSock.send("fail".encode())

			cSock.close()


	def setUsers(self):
		u1 = ("5","1234")
		u2 = ("08","8888")
		u3 = ("8", "987")

		#TODO: populate these (userid,passcode) tuples from the database
		users = [u1, u2, u3]

		return users


if __name__ == "__main__":
	auth = Authenticator(sAddr="192.168.1.106")

	auth.listenForAuth()