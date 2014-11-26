#!/ust/bin/python3

from socket import socket, AF_INET, SOCK_STREAM

class Server(object):

	def __init__(self, sAddr, sPort, buffSize):
		self.sock = socket(AF_INET, SOCK_STREAM)
		self.buffSize = buffSize
		self.sock.bind((sAddr, sPort))