import codecs
import sqlite3
import datetime
import binascii

hex_db = '/var/jail/home/team20/webserver/hex.db'

def update_database(hexx):
    conn = sqlite3.connect(hex_db)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # move cursor into database (allows us to execute commands)
    c.execute('''CREATE TABLE IF NOT EXISTS hex_table (hexx text, timing timestamp);''') # run a CREATE TABLE command
    c.execute('''INSERT into hex_table VALUES (?,?);''', (hexx, datetime.datetime.now()))
    conn.commit()
    conn.close()

def get_database(): #room (int), fake(int), stat (string), targ (int), mode(string)
    fin = []
    conn = sqlite3.connect(hex_db)
    c = conn.cursor()
    h = c.execute('''SELECT hexx FROM hex_table ORDER BY timing DESC;''').fetchall()
    return [x[0] for x in h if all(c in '0123456789abcdef' for c in x[0].lower())]

def read_hex(hexx):
    data = binascii.a2b_hex(hexx)
    with open('/var/jail/home/team20/webserver/imagetest.jpeg', 'wb') as image_file:
        image_file.write(data)

def display():
    html = codecs.open("/var/jail/home/team20/webserver/render.html", 'r')
    return html.read()


def request_handler(request):
    if request["method"] == "POST": #updating values in database
        hexx = request['form']['hexcode']
        correct_hexx = 'ffd8'+hexx+'d9' #minor edits to hex code to make it correct ffd8, ffd9
        update_database(correct_hexx)
        return 'ok'
    elif request["method"] == "GET":
        hexx = get_database()
        #return '\n'.join(hexx)
        return ''.join(f'''<img src="data:image/jpeg;base64,{codecs.encode(codecs.decode(hex, 'hex'), 'base64').decode()}" /><br />''' for hex in hexx)
