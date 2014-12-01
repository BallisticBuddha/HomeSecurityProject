import psycopg2
import sys

class PSQLConn(object):
    
    def __init__(self, djangoSettings="/var/www/HSS_Site/AC3site", dbConfig="default"):
        sys.path.append(djangoSettings)

        from settings import DATABASES

        self.dbName = DATABASES[dbConfig]["NAME"]
        self.username = DATABASES[dbConfig]["USER"]
        self.passwd = DATABASES[dbConfig]["PASSWORD"]
        self.host = DATABASES[dbConfig]["HOST"]
        self.port = DATABASES[dbConfig]["PORT"]


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
        
