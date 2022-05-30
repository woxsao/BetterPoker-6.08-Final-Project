import codecs
import sqlite3
import datetime

location_db = '/var/jail/home/team20/webserver/location.db'

def request_handler(request):
    if request["method"] == "POST":
        try:
            dorm_name = request['form']['dorm_location']
        except:
            return "invaid dorm location"
        conn = sqlite3.connect(location_db)
        c = conn.cursor()
        outs = ""
        c.execute('''CREATE TABLE IF NOT EXISTS location_table (location text, timing timestamp);''')
        c.execute('''INSERT into location_table VALUES (?,?);''', (dorm_name, datetime.datetime.now()))
        conn.commit() # commit commands
        conn.close() # close connection to database
        return (dorm_name)
    elif request['method'] == "GET":
        conn1 = sqlite3.connect(location_db)
        c1 = conn1.cursor()
        outs = []
        things1 = c1.execute('''SELECT location FROM location_table WHERE timing = (SELECT MAX(timing) FROM location_table);''')
        outs = ""
        for x in things1:
            outs+=str(x)
        conn1.commit() # commit commands
        conn1.close() # close connection to database
        return parse_output(outs)


#outs looks like --> ('next house',)
def parse_output(output):
    return output[2:-3].strip('\n')


print(parse_output("('next house',)"))