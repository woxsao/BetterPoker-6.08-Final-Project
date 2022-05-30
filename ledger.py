import requests
import sqlite3

DB = '/var/jail/home/team20/webserver/ledger.db'
DB2 = '/var/jail/home/team20/webserver/leaderboard.db'

def gameRunning():
    conn = sqlite3.connect(DB2)
    c = conn.cursor()
    thing = c.execute('''SELECT name FROM sqlite_master WHERE type='table' AND name='stax_current';''').fetchone()
    conn.commit()
    conn.close()
    return True if thing else False

def queryAll():
    conn = sqlite3.connect(DB)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS ledge (username text, net int, PRIMARY KEY(username));''')
    things1 = dict(c.execute('''SELECT * FROM ledge ORDER BY net DESC''').fetchall())
    conn.commit()
    conn.close()
    conn = sqlite3.connect(DB2)
    c = conn.cursor()
    things2 = dict(c.execute('''SELECT * FROM stax_current''').fetchall())
    conn.commit()
    conn.close()
    l = list({key: things1.get(key, 0)+10*things2.get(key, 0) for key in things1.keys() | things2.keys()}.items())
    l.sort(key=lambda x: -x[1])
    return l

def query(username):
    conn = sqlite3.connect(DB)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS ledge (username text, net int, PRIMARY KEY(username));''')
    thing = c.execute('''SELECT net FROM ledge WHERE username = ?;''', (username,)).fetchone()
    conn.commit()
    conn.close()
    return thing[0] if thing else 0

def update(username, delta):
    net = query(username)
    net += delta
    conn = sqlite3.connect(DB)
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS ledge (username text, net int, PRIMARY KEY(username));''')
    c.execute('''REPLACE INTO ledge VALUES (?, ?);''', (username, net))
    conn.commit()
    conn.close()
    return net

def reset(username):
    conn = sqlite3.connect(DB)
    c = conn.cursor()
    c.execute('''DELETE FROM ledge WHERE username = ?;''', (username,));
    conn.commit()
    conn.close()
    conn = sqlite3.connect(DB2)
    c = conn.cursor()
    c.execute('''DELETE FROM stax_current WHERE username = ?;''', (username,));
    conn.commit()
    conn.close()
    return

def request_handler(request):
    if request['method'] == 'GET':
        pass
    elif request['method'] == 'POST':
        if 'reset' in request['form']:
            reset(request['form']['reset'])
        else:
            if 'username' not in request['form']:
                return 'You must enter a username'
            if 'delta' not in request['form']:
                return 'Missing delta'
            username = request['form']['username']
            delta = int(request['form']['delta'])
            update(username, delta)
    else:
        return "Method type not supported"
    if gameRunning():
        return "<h1><a vlink= \"#000000\" href=\"http://608dev-2.net/sandbox/sc/team20/webserver/updated_website.py\">PokerTracker</a></h1>\n Ledger:<br/><ol>" + ''.join(f"<li>{x[0]}: ${x[1]/100:.2f}<form method=\"post\"><input type=\"hidden\" name=\"reset\" value=\"{x[0]}\" /><button type=\"submit\">Buy out</button></form></li>" for x in queryAll())+'</ol>'
    return "<h1><a vlink= "#000000" href="http://608dev-2.net/sandbox/sc/team20/webserver/updated_website.py">PokerTracker</a></h1><p>Game is not running</p>"

