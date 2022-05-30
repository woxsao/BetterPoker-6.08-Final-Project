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

    c.execute('''CREATE TABLE IF NOT EXISTS buzzer_table (user text, location text, claimed integer, timing timestamp);''')

    try: #specifically for buzzer
        method = request['method']
        user = request['values']['user']
        loc = request['values']['loc']
    except:
        html_home = codecs.open('/var/jail/home/team20/webserver/main.html', 'r')
        return html_home.read()

    if user == '' or loc == '':
        return "you need to add a username and location"
    #claimed is set to false when initialized, will be set to true if notification is been cancelled
    c.execute('''INSERT into buzzer_table VALUES (?,?,?,?);''',(user, loc, 0, datetime.datetime.now()))
    conn.commit()
    conn.close()



    html = codecs.open('/var/jail/home/team20/webserver/buzzer_response.html', 'r')
    return html.read()
