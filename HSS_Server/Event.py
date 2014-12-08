#!/usr/bin/python3

from psycopg2 import Error as PSQLErr

from datetime import datetime
from PSQL import PSQLConn

class Event(object):

    def __init__(self, dbConfig, eType, conStartTime, timeRecvd, seq):
        self.psql = PSQLConn(dbConfig)

        self.eventType = eType
        self.started = conStartTime
        self.dateReceived = timeRecvd.date()
        self.timeReceived= timeRecvd.time()
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

        received = datetime.combine(self.dateReceived, self.timeReceived)
        duration = received - self.started

        ret = """[Event Consumer] Event received. Transmission took %s seconds.
        \tTime Received: %s
        \tSequence Number: %i
        \tEvent Type: %s
        \tUserID: %s
        \tSensor Triggered: %s
        \tPicture Format: %s""" % (str(duration), str(received), self.seqNum, etStr, 
            str(self.user), str(self.sensor), pFmt)
        if self.imgType:
            ret += "\n\tPicture Size: %s bytes" % self.imgSize
            ret += "\n\tPicture Location: %s" % self.imgPath

        return ret


    def setSensor(self, sArr):
        sensor = None

        for sNum in range(0, len(sArr)):
            if sArr[sNum]:
                sensor = sNum + 1
                break

        self.sensor = sensor


    def __getProfileID(self, uid):
        ret = None

        if uid > 0: # 0 is not a valid user ID
            try:
                with self.psql as cursor:
                    cursor.execute("SELECT id FROM ac3app_userprofile WHERE (user_id = %s)", (uid,))
                    resTup = cursor.fetchone()

                    if resTup and resTup[0]:
                        ret = resTup[0]
            except PSQLErr as e:
                print("[Event Consumer] Failed to read from database...")
                print(e)

        return ret


    def setUser(self, uid):
        self.user = self.__getProfileID(uid)


    def setImage(self, iType, iSize, iPath):
        self.imgType = iType
        self.imgSize = iSize
        self.imgPath = iPath


    def storeEvent(self):
        try:
            with self.psql as cursor:
                cursor.execute("INSERT INTO ac3app_event (date_created, time_created, event_type_id, \
                    sensor_triggered_id, user_id_id, event_image) VALUES (%s , %s, %s, %s, %s, %s)", 
                    (self.dateReceived, self.timeReceived, self.eventType, self.sensor, self.user, self.imgPath))
        except PSQLErr as e:
            print("[Event Consumer] Failed to insert into database, event not logged...")
            print(e)

