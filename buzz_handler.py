
import sqlite3
import json
import datetime
visits_db = '/var/jail/home/team20/webserver/buzz.db'


def request_handler(request):
    if request['method'] == "GET":
        conn = sqlite3.connect(visits_db) #UNCOMMENT WHEN ON SERVER SIDE
        c = conn.cursor()  # move cursor into database (allows us to execute commands)
        c.execute('''CREATE TABLE IF NOT EXISTS buzzer_table (user text, location text, claimed integer, timing timestamp);''')
        yesterday = datetime.datetime.now()-datetime.timedelta(days = 1)
        c.execute("DELETE FROM buzzer_table WHERE timing < ?", (yesterday,))
        name = c.execute("SELECT user, location FROM buzzer_table WHERE claimed = 0;").fetchone()
        conn.commit()
        conn.close()
        if name:
           # return name
            return_dict = {"username": name[0], "location": name[1]}
        else:
            return_dict = {"username": "-", "location": "-"}
        return json.dumps(return_dict)
    else:
        body = json.loads(request['data'])
        if 'username' not in body:
            return 'You must enter a username'
        username = body['username']
        conn = sqlite3.connect(visits_db) #UNCOMMENT WHEN ON SERVER SIDE
        c = conn.cursor()  # move cursor into database (allows us to execute commands)
        c.execute("UPDATE buzzer_table SET claimed = 1 WHERE user = ?;", (username,))
        conn.commit()
        conn.close()
        return "acknowledged"
