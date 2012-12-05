import pyodbc

class mysql:
    
    def __init__(self, sid):    
        cncx = pyodbc.connect('DSN=%s' % sid)         # 'driver={MySQL};database=' + param['database'] + ';server=' + param['host'] + ';user=' + param['user'] + ';password=' + param['password'] + ';charset=' + param['charset']
        self.cur = cncx.cursor()
        pass
    
    def query(self, sql):
        self.cur.execute(sql)
        return self.cur
