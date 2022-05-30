import requests
import sqlite3
import datetime

DB = '/var/jail/home/team20/webserver/leaderboard.db'

def gameRunning():
    conn = sqlite3.connect(DB)
    c = conn.cursor()
    thing = c.execute('''SELECT name FROM sqlite_master WHERE type='table' AND name='stax_current';''').fetchone()
    conn.commit()
    conn.close()
    return True if thing else False

def queryAll(leaderboard):
    table_name = 'stax_' + leaderboard
    conn = sqlite3.connect(DB)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS {} (username text, stack int, PRIMARY KEY(username));'''.format(table_name))
    things = c.execute('''SELECT * FROM {} ORDER BY stack DESC'''.format(table_name)).fetchall()
    conn.commit()
    conn.close()
    return things

def startGame():
    conn = sqlite3.connect(DB)
    c = conn.cursor()
    c.execute('''CREATE TABLE stax_current (username text, stack int, PRIMARY KEY(username));''')
    conn.commit()
    conn.close()
    return

def endGame():
    conn = sqlite3.connect(DB)
    c = conn.cursor()
    c.execute('''DROP TABLE stax_current;''');
    conn.commit()
    conn.close()
    return

def query(leaderboard, username):
    table_name = 'stax_' + leaderboard
    conn = sqlite3.connect(DB)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS {} (username text, stack int, PRIMARY KEY(username));'''.format(table_name))
    thing = c.execute('''SELECT stack FROM {} WHERE username = ?;'''.format(table_name), (username,)).fetchone()
    conn.commit()
    conn.close()
    return thing[0] if thing else 0

def pot_query():
    fin = []
    conn = sqlite3.connect(DB)
    c = conn.cursor()
    thing = c.execute('''SELECT pot FROM pot_tabl WHERE timing = (SELECT MAX(timing) FROM pot_tabl);''')
    for x in thing:
        fin.append(x)
    return fin[0][0]

def update(leaderboard, username, delta):
    table_name = 'stax_' + leaderboard
    stack = query(leaderboard, username)
    stack += delta
    conn = sqlite3.connect(DB)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS {} (username text, stack int, PRIMARY KEY(username));'''.format(table_name))
    c.execute('''REPLACE INTO {} VALUES (?, ?);'''.format(table_name), (username, stack))
    conn.commit()
    conn.close()
    return stack

def update_pot(delta):
    delta = delta * -1
    conn = sqlite3.connect(DB)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS pot_tabl (pot real, timing timestamp);''')
    newpot = int(pot_query()) + delta
    c.execute('''INSERT into pot_tabl VALUES (?,?);''', (newpot, datetime.datetime.now()))
    conn.commit()
    conn.close()

def request_handler(request):
    if request['method'] == 'GET':
        if 'leaderboard' in request['values']:
            leaderboard = request['values']['leaderboard']
            if leaderboard not in ['total', 'current']:
                return "Invalid leaderboard"
            if 'username' in request['values']:
                return query(leaderboard, request['values']['username'])
            if leaderboard == 'current' and not gameRunning():
                return "Game has not started"
            return "<h1><a vlink= \"#000000\" href=\"http://608dev-2.net/sandbox/sc/team20/webserver/updated_website.py\">BetterPoker</a></h1>\n" +leaderboard + ' Leaderboard:<br/><ol>' + ''.join(f"<li>{x[0]}: {x[1]}</li>" for x in queryAll(leaderboard))+'</ol>'
    elif request['method'] == 'POST':
        if 'game' in request['form']:
            if 'end' in request['form']:
                endGame()
            else:
                startGame()
        else:
            if 'username' not in request['form']:
                return 'You must enter a username'
            if 'delta' not in request['form']:
                return 'Missing delta'
            username = request['form']['username']
            delta = int(request['form']['delta'])
            update('total', username, delta)
            update('current', username, delta)
            update_pot(delta)
    else:
        return "Method type not supported"
    running = gameRunning()
    return f'''
        The default HTML<br/>
        Game status: {'Running' if running else 'Not running'}<br/>
        <form method="post">
        <input type="hidden" name="game" />
        <input type="submit" value="{'End Game' if running else 'Start Game'}" />
        </form>
        {
            """
                <br />
                <form method="post">
                <label for="username">Username:</label>
                <input type="text" id="username" name="username" />
                <br />
                <label for="delta">Delta:</label>
                <input type="number" id="delta" name="delta" value="0" />
                <br />
                <input type="submit" value="Submit" />
                </form>
            """ if running else ''
        }
    '''



