import codecs
import sqlite3
import datetime
visits_db = '/var/jail/home/team20/webserver/buzz.db'

def request_handler(request):
    """
    Name of database: buzz_db
    user(str), location(str), claimed(int) time(timestamp)
    """
    conn = sqlite3.connect(visits_db) #UNCOMMENT WHEN ON SERVER SIDE 
    c = conn.cursor()  # move cursor into database (allows us to execute commands)

    """
    READ THE DATABASE FOR ANY USERS WITH A 0 IN THE CLAIMED COLUMN AND SENDS IT AS A GET REQUEST BACK TO THE ESP
    """
    names = c.execute('''SELECT DISTINCT user FROM buzzer_table WHERE claimed=0;''')
    lis_name = [x[0] for x in names]
    try: #specifically for buzzer
        if "silence" in request['values']:
            c.execute('''UPDATE buzzer_table SET claimed=1 WHERE claimed=0;''')
            conn.commit()
            conn.close()
            return []
        else:
            return lis_name
    except:
        print("Something went wrong")