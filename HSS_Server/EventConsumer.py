#!/usr/bin python

from Server import Server

class EventConsumer(Server):

	def __init__(self, sAddr, sPort, buffsize):
		Server.__init__(self, sAddr, sPort, buffSize)
		self.running = True
		print("Event Consumer Server started...")
		