import codecs
import sqlite3
import datetime

location_db = '/var/jail/home/team20/webserver/location.db'
buzz_db = '/var/jail/home/team20/webserver/buzz.db'


def request_handler(request):
    dorm = retrieve_dorm_name()
    if gameRunning():
        location_html_msg = "<p>There is currently a game going on in: <b>" + dorm + "</b></p>"
    else:
        location_html_msg = "<p>There are no games going on right now</p>"
    html_output = """
    <!DOCTYPE html>
    <html>
        <head>
            <title>BetterPoker Web Interface</title>
        </head>
        <body>
            <h2>Welcome To BetterPoker</h2>
            """+ location_html_msg+"""
            <a href="http://608dev-2.net/sandbox/sc/team20/webserver/buzzer.html">Buzz In</a>
            </br>
            <a href="http://608dev-2.net/sandbox/sc/team20/webserver/leaderboard.py?leaderboard=total">All Time Leaderboard</a>
            </br>
            <a href="http://608dev-2.net/sandbox/sc/team20/webserver/leaderboard.py?leaderboard=current">Current Leaderboard</a>
            </br>
            <a href = "http://608dev-2.net/sandbox/sc/team20/webserver/hex.py"> Photo Gallery</a>
	    </br>
	    <a href = "http://608dev-2.net/sandbox/sc/team20/webserver/ledger.py"> Ledger</a>
        </body>
    </html>
    """
    return html_output

def retrieve_dorm_name():
    conn1 = sqlite3.connect(location_db)
    c1 = conn1.cursor()
    outs = []
    things1 = c1.execute('''SELECT location FROM location_table WHERE timing = (SELECT MAX(timing) FROM location_table);''')
    outs = ""
    for x in things1:
        outs+=str(x)
    conn1.commit() # commit commands
    conn1.close() # close connection to database
    return outs[2:-3]

# no longer displays the users who are in the game as this can be seen through the leaderboard 

def get_current_user():
    conn1 = sqlite3.connect(buzz_db)
    c1 = conn1.cursor()
    outs = []
    things1 = c1.execute('''SELECT DISTINCT user FROM buzzer_table;''')
    outs = []
    for x in things1:
        outs.append(str(x)[2:-3])
    conn1.commit() # commit commands
    conn1.close() # close connection to database
    return outs

def get_current_user_in_dorm(dorm_name):
    conn1 = sqlite3.connect(buzz_db)
    c1 = conn1.cursor()
    outs = []
    things1 = c1.execute('''SELECT DISTINCT user FROM buzzer_table where location=?;''', [dorm_name])
    outs = []
    for x in things1:
        outs.append(str(x)[2:-3])
    conn1.commit() # commit commands
    conn1.close() # close connection to database
    return outs

DB = '/var/jail/home/team20/webserver/leaderboard.db'

def gameRunning():
    conn = sqlite3.connect(DB)
    c = conn.cursor()
    thing = c.execute('''SELECT name FROM sqlite_master WHERE type='table' AND name='stax_current';''').fetchone()
    conn.commit()
    conn.close()
    return True if thing else False
