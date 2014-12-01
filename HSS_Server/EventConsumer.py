#!/usr/bin python

import os
from datetime import datetime

from Server import Server

class EventConsumer(Server):

	def __init__(self, sAddr='127.0.0.1', sPort=8089, buffSize=1024):
		Server.__init__(self, sAddr, sPort, buffSize)
		self.running = True
		self.picLib = os.getcwd() + "/images/"
		if not os.path.isdir(self.picLib):
			os.makedirs(self.picLib)

		print("Event Consumer Server started...")
		

	def getACK(self, eType, seq = None):
		ack = []
		ack.append(3 << 6) # event ACK packet type
		ack[0] = ack[0] | (eType << 4) # type of event acked
		ack.extend([0 for _ in range(0,3)]) # add 3 bytes of padding

		if isinstance(seq, int):
			if seq <= 0xFFFFFFFF and seq >= 0:
				ack.append(seq >> 24)
				ack.append((seq & 0x00FF0000) >> 16)
				ack.append((seq & 0x0000FF00) >> 8)
				ack.append(seq & 0x000000FF)
			else:
				print("Invalid sequence number %i" % seq)

		return bytes(ack)


	def getHBResponse(self):
		resByte = 1 << 6 # heartbeat packet type
		resByte = resByte | (2 << 4) # response heartbeat type

		if self.running:
			resByte = resByte | 1 # 1 for alive

		return bytes([resByte])


	def getImageName(self):
		filename = ""
		for i in range(0, 10000):
			filename = "img%04d.jpg" % i
			if not os.path.exists(self.picLib + filename):
				break

		return self.picLib + filename


	def consumeEvents(self):
		while self.running:
			self.sock.listen(0)
			cSock, cAddr = self.sock.accept()
			print("[Event Consumer] Accepted connection from %s." % str(cAddr))
			connAlive = True
			event = []
			pictureSize = None
			filename = None

			timeStarted = datetime.now();
			try:
				while connAlive:
					data = cSock.recv(self.buffSize)

					event.extend(data)

					if (event[0] >> 6) == 1: # heartbeat message
						connAlive = False

					if (event[0] & 0x0F) == 0: # no picture (should be exactly 8 bytes)
						if len(event) >= 8:
							connAlive = False
					elif len(event) >= 12:
						pictureSize = event[8] << 24
						pictureSize = pictureSize | (event[9] << 16)
						pictureSize = pictureSize | (event[10] << 8)
						pictureSize = pictureSize | event[11]

						if len(event) >= pictureSize + 12:
							connAlive = False

				print("[Event Consumer] Packet received.")

				# Organize event data
				timeReceived = datetime.now()
				packetType = event[0] >> 6

				# print common data to console
				print("Time Received: %s" % str(timeReceived))
				print("Transmission took %s seconds" % (timeReceived - timeStarted) )

				if packetType == 1:
					pTypeStr = "HEARTBEAT"
				elif packetType == 2:
					pTypeStr = "EVENT"
				elif packetType == 3:
					pTypeStr = "EVENT ACK"
				else:
					pTypeStr = "INVALID"
				print("Packet Type: %s" % pTypeStr)

				if packetType == 1: # heartbeat
					hbType = (event[0] & 0x30) >> 4;
					hbMessage = event[0] & 0x0F;

					if hbType == 1:
						cSock.send(self.getHBResponse())
						cSock.close()
						hbTypeStr = "REQUEST"
					elif hbType == 2:
						hbTypeStr = "RESPONSE"
					else:
						hbTypeStr = "INVALID"

					print("Heartbeat Type: %s" % hbTypeStr)

					print("Heartbeat Message: %i" % hbMessage)
				elif packetType == 2: # event

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

					seqNum = event[4] << 24
					seqNum = seqNum | (event[5] << 16)
					seqNum = seqNum | (event[6] << 8)
					seqNum = seqNum | event[7]

					# ACK the event
					cSock.send(self.getACK(eventType, seqNum))
					cSock.close()

					if pictureSize:
						filename = self.getImageName()
						with open(filename, 'wb') as f:
							for i in range(0, pictureSize - 1):
								f.write(bytes([event[12 + i]]))

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

					# Sequence Number
					print("Sequence Number: %i" % seqNum)

					# Picture Type
					if pictureType == 1:
						pTypeStr = "JPEG"
					elif pictureType == 0:
						pTypeStr = "NONE"
					print("Picture Type: %s" % pTypeStr)

					if pictureType:
						print("Picture Size: %i bytes" % pictureSize)
						print("Picture Location: %s" % filename)
			except ConnectionResetError as e:
				print("[Event Consumer] Connection was reset, resuming to allow new connections.")
				print("[Event Consumer] %i bytes received before reset." % len(event))




