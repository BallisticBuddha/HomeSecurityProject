#!/usr/bin python

from datetime import datetime
from Server import Server

class EventConsumer(Server):

	def __init__(self, sAddr='127.0.0.1', sPort=8089, buffSize=1024):
		Server.__init__(self, sAddr, sPort, buffSize)
		self.running = True
		print("Event Consumer Server started...")
		

	def getHBResponse(self):
		resByte = 1 << 6 # heartbeat packet type
		resByte = resByte | (2 << 4) # response heartbeat type

		if self.running:
			resByte = resByte | 1 # 1 for alive

		return bytes([resByte])

	def consumeEvents(self):
		while self.running:
			self.sock.listen(0)
			cSock, cAddr = self.sock.accept()
			print("[Event Consumer] Accepted connection from %s." % str(cAddr))
			connAlive = True
			event = []
			pictureSize = None
			filename = None

			while connAlive:
				data = cSock.recv(self.buffSize)

				for c in data:
					event.append(c)

				if (event[0] >> 6) == 1: # heartbeat message
					connAlive = False

				if (event[0] & 0x0F) == 0: # no picture (should be exactly 4 bytes)
					if len(event) >= 4:
						connAlive = False
				elif len(event) >= 8:
					pictureSize = event[4] << 24
					pictureSize = pictureSize | (event[5] << 16)
					pictureSize = pictureSize | (event[6] << 8)
					pictureSize = pictureSize | event[7]

					if len(event) >= pictureSize + 8:
						connAlive = False

			print("[Event Consumer] Packet recieved.")

			# Organize event data
			timeReceived = datetime.now()
			packetType = event[0] >> 6

			# print common data to console
			print("Time Received: %s" % str(timeReceived))

			if packetType == 1:
				pTypeStr = "HEARTBEAT"
			elif packetType == 2:
				pTypeStr = "EVENT"
			else:
				pTypeStr = "INVALID"
			print("Packet Type: %s" % pTypeStr)

			if packetType == 1: # heartbeat
				hbType = (event[0] & 0x30) >> 4;
				hbMessage = event[0] & 0x0F;

				if hbType == 1:
					cSock.send(self.getHBResponse())
					hbTypeStr = "REQUEST"
				elif hbType == 2:
					hbTypeStr = "RESPONSE"
				else:
					hbTypeStr = "INVALID"

				print("Heartbeat Type: %s" % hbTypeStr)

				print("Heartbeat Message: %i" % hbMessage)

			# This is (I think) the earliest we can close the socket
			cSock.close()


			if packetType == 2: # event
				eventType = (event[0] & 0x30) >> 4
				pictureType = event[0] & 0x0F
				if event[1] == 0 and event[2] == 0:
					userID = None
				elif event[1] == 0:
					userID = chr(event[2])
				else:
					userID = chr(event[1]) + chr(event[2])

				sensors = [ False for _ in range(0,8) ]

				sensors[0] = ((event[3] & 128) >> 7) == 1
				sensors[1] = ((event[3] & 64) >> 6) == 1
				sensors[2] = ((event[3] & 32) >> 5) == 1
				sensors[3] = ((event[3] & 16) >> 4) == 1
				sensors[4] = ((event[3] & 8) >> 3) == 1
				sensors[5] = ((event[3] & 4) >> 2) == 1
				sensors[6] = ((event[3] & 2) >> 1) == 1
				sensors[7] = (event[3] & 1) == 1

				if pictureSize:
					filename = "sample.jpg"
					with open(filename, 'wb') as f:
						for i in range(0, pictureSize - 1):
							f.write(event[8 + i])

				# Print event data to console

				# Event Type
				if eventType == 1:
					eTypeStr = "ARM"
				elif eventType == 2:
					eTypeStr = "DISARM"
				elif eventType == 3:
					eTypeStr = "ALARM"
				else:
					eTypeStr = "INVALID"
				print("Event Type: %s" % eTypeStr)


				# User ID
				print("UserID: %s" % userID)

				# Sensors Triggered
				print("Sensors Triggered:")
				for i in range(0,len(sensors)):
					print("\tSensor %i: %s" % (i + 1, sensors[i]))

				# Picture Type
				if pictureType == 1:
					pTypeStr = "JPEG"
				elif pictureType == 0:
					pTypeStr = "NONE"
				print("Picture Type: %s" % pTypeStr)

				if pictureType:
					print("Picture Size: %i bytes" % pictureSize)
					print("Picture Location: %s" % filename)




