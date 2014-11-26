#!/usr/bin/python3

from threading import Thread

from EventConsumer import EventConsumer
from Authenticator import Authenticator

authServ = Authenticator(sAddr='192.168.1.106', sPort=8088)
eventServ = EventConsumer(sAddr='192.168.1.106', sPort=8089)

t1 = Thread(target=authServ.listenForAuth)
t2 = Thread(target=eventServ.consumeEvents)

t1.start()
t2.start()