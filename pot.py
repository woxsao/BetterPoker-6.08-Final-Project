import requests
import sqlite3
import datetime

DB = '/var/jail/home/team20/webserver/leaderboard.db'

def reset():
    conn = sqlite3.connect(DB)
    c = conn.cursor()
    newpot = 0
    c.execute('''CREATE TABLE IF NOT EXISTS pot_tabl (pot real, timing timestamp);''')
    c.execute('''INSERT into pot_tabl VALUES (?,?);''', (newpot, datetime.datetime.now()))
    conn.commit()
    conn.close()
    return newpot

def pot_query():
    fin = []
    conn = sqlite3.connect(DB)
    c = conn.cursor()
    thing = c.execute('''SELECT pot FROM pot_tabl WHERE timing = (SELECT MAX(timing) FROM pot_tabl);''')
    for x in thing:
        fin.append(x)
    return fin[0][0]

def request_handler(request):
    if request['method'] == 'GET':
        return int(pot_query())
    elif request['method'] == 'POST':
        return reset()
    else:
        return "Method type not supported"
       
