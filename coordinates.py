def bounding_box(point_coord,box):
    x = point_coord[0]
    y = point_coord[1]
    x_vals = []
    y_vals = []
    for i in box:
        x_vals.append(i[0])
        y_vals.append(i[1])
    return(x > min(x_vals) and x < max(x_vals) and y > min(y_vals) and y < max(y_vals))

def translate(point, origin):
    return (point[0] - origin[0], point[1] - origin[1])

def within_area(point_coord,poly):
    x_vals = []
    y_vals = []
    count = 0
    x = point_coord[0]
    y = point_coord[1]
    offset_x = 0-x
    offset_y = 0-y
    for i in poly:
        x_vals.append(i[0])
        y_vals.append(i[1])
    for i in range(len(x_vals)):
        x_vals[i] += offset_x
        y_vals[i] += offset_y
    for i in range(len(x_vals)-1):
        if sign(y_vals[i]) != sign(y_vals[i+1]):
            if sign(x_vals[i]) == 1 and sign(x_vals[i+1]) == 1:
                count += 1
            if sign(x_vals[i]) != sign(x_vals[i+1]):
                x_int = (x_vals[i]*y_vals[i+1] - y_vals[i]*x_vals[i+1])/(y_vals[i+1]-y_vals[i])
                if sign(x_int) > 0:
                    count += 1
    last_idx = len(x_vals)-1
    if sign(y_vals[last_idx]) != sign(y_vals[0]):
            if sign(x_vals[last_idx]) == 1 and sign(x_vals[0]) == 1:
                count += 1
            if sign(x_vals[last_idx] != sign(x_vals[0])):
                x_int = (x_vals[last_idx]*y_vals[0] - y_vals[last_idx]*x_vals[0])/(y_vals[0]-y_vals[last_idx])
                if sign(x_int) > 0:
                    count += 1
    return (count%2 != 0)

locations={
    "Next House": [(-71.10271626278049, 42.35488146279714), (-71.1023783044712, 42.35428682543975), (-71.10103988227803, 42.354689197333755), (-71.10136442954331, 42.35524418882237)],
    "New House": [(-71.10129660799082, 42.355177318606536), (-71.10105040852257, 42.35472871709715), (-71.10002635217147, 42.35503807022128), (-71.10008928145243, 42.35515855709686), (-71.09986006004986, 42.35522362734619), (-71.0999735018047, 42.35539980142035), (-71.10009213075337, 42.35536035241603), (-71.10020692384337, 42.35554655715635)],
    "MacGregor House": [(-71.10008877977086, 42.355588647505), (-71.09997411534448, 42.355393906581426), (-71.09995131656964, 42.3553993573534), (-71.0998068599033, 42.355152608767916), (-71.09900052089358, 42.35541394347172), (-71.09929228640864, 42.35584666266434)],
    "Number Six Club": [(-71.09893178666526, 42.355816950378674), (-71.09875693300988, 42.3555190636626), (-71.09861268050457, 42.355579763174816), (-71.09878501242022, 42.35587162570955)],
    "Burton Conner": [(-71.09878583442256, 42.35588530633024), (-71.098618345706, 42.355590508325946), (-71.09739767965162, 42.355990138975336), (-71.09761652091883, 42.3563458571594), (-71.0979102403081, 42.35635595248425), (-71.09841798777943, 42.35619106197518), (-71.09834285026125, 42.356031218721775)],
    "Greek Life Housing": [(-71.09755589811101, 42.356261475320444), (-71.09740559399003, 42.35597837087846), (-71.09668006582706, 42.35618297451153), (-71.09686879355792, 42.35646774825527)],
    "Theta Delta Chi": [(-71.09652711669077, 42.35626833153348), (-71.09630335566852, 42.35633848110188), (-71.09641297596728, 42.35653139201129), (-71.09663899720188, 42.356462912881796)],
    "Baker House": [(-71.0962865218809, 42.35630622218379), (-71.09507416346345, 42.35671056320609), (-71.09530483342783, 42.357021745456564), (-71.09647964092083, 42.356680832337176)], 
    "Green Hall": [(-71.09529416418113, 42.357046658197405), (-71.09509686989543, 42.35674409838202), (-71.09492715438085, 42.35680523752952), (-71.0950947484515, 42.35709682341462)],
    "Simmons Hall": [(-71.10225206806263, 42.35689497114481), (-71.1020553357299, 42.35671082564841), (-71.10079887189819, 42.357342733205826), (-71.10097986564429, 42.35750361663043)],
    "New Vassar": [(-71.09841646578639, 42.35889270297052), (-71.09816965732108, 42.358620608566234), (-71.09680914647203, 42.359288772042675), (-71.0970260990746, 42.35951968221149)],
    "McCormick Hall": [(-71.09502085938502, 42.35727475988477), (-71.09476087395126, 42.35684071330469), (-71.09376586797022, 42.35717751473671), (-71.09407078915795, 42.357528545294045)],
    "Maseeh Hall": [(-71.09397403458492, 42.35781436556865), (-71.09363777464146, 42.35724370164347), (-71.09291396086486, 42.357454279340956), (-71.09322742352403, 42.35805442190745)],
    "Random Hall": [(-71.0985728864783, 42.361833492695055), (-71.09822746452194, 42.361636974343405), (-71.09797772282579, 42.36187423422762), (-71.09831503628553, 42.36205277769112)]
}

def sign(x):
    if x > 0:
        return 1
    elif x == 0:
        return 0
    else:
        return -1

def get_area(point_coord,locations):
    for k,v in locations.items():
        if (within_area(point_coord, v)):
            return k
    return "Off Campus"

import sqlite3
import datetime
location_db = '/var/jail/home/team20/final_project/location.db'

def request_handler(request):
    if request["method"] == "GET":
        lat = 0
        lon = 0
        try:
            lat = float(request['values']['lat'])
            lon = float(request['values']['lon'])
        except Exception as e:
            # return e here or use your own custom return message for error catch
            #be careful just copy-pasting the try except as it stands since it will catch *all* Exceptions not just ones related to number conversion.
            return "Error: lat, lon are missing or not numbers"
        return get_area([lon,lat],locations)
    else:
        try:
            username = request['form']['user']
            lon = float(request['form']['lon'])
            lat = float(request['form']['lat'])
        except:
            return 'POST not supported. You need to change that.'

        conn = sqlite3.connect(location_db)
        c = conn.cursor()
        outs = ""
        c.execute('''CREATE TABLE IF NOT EXISTS location_table (user text, lat float, long float, location text, timing timestamp);''')
        thirty_seconds_ago = datetime.datetime.now() - datetime.timedelta(seconds = 30)
        location = get_area((lon,lat),locations)
        c.execute('''INSERT into location_table VALUES (?,?,?,?,?);''',(username, lon, lat, get_area((lon,lat),locations), datetime.datetime.now()))
        things = c.execute('''SELECT * FROM location_table;''').fetchall()
        outs = location + "\n"
        for x in things:
            outs+=str(x[0])+"\r\n"
        conn.commit() # commit commands
        conn.close() # close connection to database
        return outs

# for testing purposes only
# test_list = []
# str = "42.354940, -71.102136"
# com_idx = str.find(",")
# lon = float(str[0:com_idx])
# lat = float(str[com_idx+1:])
# test_list.append(lat)
# test_list.append(lon)
# new_coord = tuple(test_list)
# print(new_coord)

# x= get_area(new_coord, locations)
# print(x)
