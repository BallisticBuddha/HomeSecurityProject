import psycopg2
import sys

class PSQLConn(object):
    
    def __init__(self, dbSettings):
        sys.path.append(dbSettings["configDir"])

        from settings import DATABASES

        self.dbName = DATABASES[dbSettings["configName"]]["NAME"]
        self.username = DATABASES[dbSettings["configName"]]["USER"]
        self.passwd = DATABASES[dbSettings["configName"]]["PASSWORD"]
        self.host = DATABASES[dbSettings["configName"]]["HOST"]
        self.port = DATABASES[dbSettings["configName"]]["PORT"]


    def __enter__(self):
        self.conn = psycopg2.connect(database=self.dbName, user=self.username, 
            password=self.passwd, host=self.host, port=self.port)
        return self.conn.cursor()


    def __exit__(self, type, value, tb):
        self.commit()
        self.conn.close()


    def commit(self):
        return self.conn.commit()


    def rollback(self):
        return self.conn.rollback()
        
