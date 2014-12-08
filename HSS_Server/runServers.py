#!/usr/bin/python3

import yaml
from threading import Thread

from EventConsumer import EventConsumer
from Authenticator import Authenticator

def getSettings(configFile):
	yamlFile = open(configFile, "r")
	return yaml.load(yamlFile)

config = getSettings("config.yaml")["HSS_Server"]
authServ = Authenticator(config)
eventServ = EventConsumer(config)

t1 = Thread(target=authServ.listenForAuth)
t2 = Thread(target=eventServ.consumeEvents)

t1.start()
t2.start()
