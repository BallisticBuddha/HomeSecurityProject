#!/usr/bin/python3

from datetime import datetime
from PSQL import PSQLConn

class Event(object):

    def __init__(self, eType, conStartTime, timeRecvd, seq):
        self.psql = PSQLConn()

        self.eventType = eType
        self.started = conStartTime
        self.dateReceived = timeRecvd.date
        self.timeReceived= timeRecvd.time
        self.seqNum = seq
        self.sensor = None
        self.user = None
        self.imgType = None
        self.imgSize = None
        self.imgPath = None


    def __str__(self):
        if self.eventType == 1:
            etStr = "ARM"
        elif self.eventType == 2:
            etStr = "DISARM"
        elif self.eventType == 3:
            etStr = "ALARM"
        else:
            etStr = "INVALID"

        if self.imgType == 1:
            pFmt = "JPEG"
        elif self.imgType == 0:
            pFmt = "NONE"
        else:
            pFmt = "INVALID"

        received = self.dateReceived + self.timeReceived
        duration = created - self.started

        ret = """[Event Consumer] Event received. Transmission took %s seconds.
        \tTime Received: %s
        \tSequence Number: %i
        \tEvent Type: %s
        \tUserID: %i
        \tSensor Triggered: %s
        \tPicture Format: %s""" % (str(duration), str(received), self.seq, etStr, 
            self.user, self.sensor, pFmt)
        if self.imgType:
            ret += "\n\tPicture Size: %s bytes" % self.imgSize
            ret ++ "\n\tPicture Location: %s" % self.imgPath

        return ret


    def setSensor(self, sArr):
        sensor = None

        for sNum in range(0, len(sArr)):
            if sArr[sNum]:
                sensor = sNum + 1
                break

        self.sensor = sensor


    def setUser(self, uid):
        self.user = int(uid)


    def setImage(self, iType, iSize, iPath):
        self.imgType = iType
        self.imgSize = iSize
        self.imgPath = iPath


    def storeEvent(self):
        cols = ["date_created", "time_created", 
            "event_type_id", "sensor_triggered_id", "user_id_id", "event_image"]

        with self.psql as cursor:
            cursor.execute("INSERT INTO ac3app_event VALUES (%s, %s, %i, %i, %i, %s)" %\
             (self.eventType, self.dateCreated, self.timeCreated, self.sensor, self.user, 
             self.imgPath))

