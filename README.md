<div style="font-size: 30; font-weight: bold; text-align: center;">BetterPoker</div>

<meta charset="utf-8"/>
Many of our friends play poker, sometimes for money. While this is fun, there are many annoying facets of 
dealing with at-home poker games, the main ones being that keeping a ledger of how much money an individual 
owes or gains takes a long time, and all too often it is done at the end of the game. Sometimes people lose 
track of how many people bought in, and how many buy-ins a certain person had before they left the game. This 
leads to sometimes having chips go missing and the table doesn’t end up breaking even. Not only this, people 
hate to be stuck in the lobby of a dorm they do not live in because players are too enthralled by the game 
to check their phones. Additionally, we often like to retell our poker stories to people who weren’t at the 
game, but have a hard time recalling the board and our hands. BetterPoker is an embedded system device that 
aims to resolve these problems to improve the quality of life of dorm-based poker games with the inclusion of 
features such as:

* The ability to let players know that you need to be tapped in 
* Keeping track of the current pot size
* Taking pictures to commemorate your winning hands!


<iframe width="1512" height="723" src="https://www.youtube.com/embed/0p4siM_CO24" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
# How-to

## Joining the Game

* Buzzing in: To let the current players know that you have to be signed into the dorm, navigate to the “Buzz In” tab from the home page. Then insert your name and your current location. This will alert the current players that you have arrived.

* Central display- Buzz-in: When the player pressed the submit button on the buzz-in HTML page, the RGB matrix of the central display would display a text in the form “[username] is at [location]” and a buzzing sound would be triggered. To acknowledge or confirm that a player can be let in, someone would press the button (connected to pin45) and then the buzzing sound would stop and the central display would return to displaying the current pot size. 

* Player module: To join a game, a player would start by inputting their username on the player module through the keypad. The keypad operates like an old cellphone keyboard and once the user is done inputting their desired character they would press the # key to proceed to input the next character. Once the user is done inputting their username they would press the * key to send their username to the server and update the leaderboard. Each player is given a default stack size of 200 and a player’s current stack size should be displayed on the tft lcd display of the player module along with the player’s username and location. 

## During the Game

* Player Module: During the game, the player can modify their current stack size by inputting bet sizes through the keypad input. The keypad input at this time is in its numerical mode. When players are making a bet, they are losing chips, so they would need to input a negative sign in front of their desired value. To input a negative sign, you would press the button A. To clear the current numerical value, you would press the button B. Once you are satisfied with the new change in stack size input, you would press the * key to send this information to the server.

* Camera Module: If during the game, the user wants to take a picture of the cards, they would do so by using the camera module and pressing the button connected to pin 45. The picture taken would be displayed on the photo gallery section of the website.

## After Each Round

* Player Module: After each round, the winning user would need to input the total number of chips they have won during that hand, and input it as a positive numerical value through the keypad of the player module. The winning player would make sure to press the * key to send their changes to the server. After pressing the * key, the pot size would reset to zero. This can be seen through the RGB matrix of the central display. 

## End of a Game

* Ending a game: When the players are done with a round and want to reset the pot size, they would press the button connected to pin 39 of the central display to reset the pot size to 0. In addition, if the players are done with a game, they can press the button connected to pin 34 to end the game. If a game was ended, the website would reflect that there are no games going on at the moment and the current leaderboard would become unavailable. 


## Logistics 
* Navigating the website: On the landing page, the website displays the location of the current game. If there is no game currently going on, the website would say “There are no games going on right now.” The website also has links to other tabs which include the Buzz In, All Time Leaderboard, Current Leaderboard, Photo Gallery, and Ledger pages. To go to another page, simply click on the name of the page you’d want to be redirected to. 

* All time leaderboard: The all-time or total leaderboard shows the overall earnings or losses of the players. This is not an indicator or who is a better poker player but rather the net wins or losses of a player as they can always buy into a game. 

* Current leaderboard: Throughout the game, you can go onto the website’s current leaderboard and you can monitor the number of chips each player has in their stack. The leaderboard would only show if there is currently a game going on. 

* Photo gallery: The photo gallery tab displays all the previously taken photos during the game. 

* Buying back in: To buy back into the game, the user would do so through the keypad input on the player module. The player would press the C key to buy back in after using their whole stack. 

* Buying out: On the website, there is a ledger page that allows users to see how money each player owes or is owed. A positive number indicated that the user is owed money (went up during the game) and a negative number indicated that the player owes money (went down during the game). Underneath each user, there is a “buy out” button that resolves the user’s balance on the ledger and also buys them out of the game. This affects the current leaderboard, as after a player has bought out, their username would be removed from the current leaderboard. 


# Documentation
The purpose of this project is to make tracking at-home poker games easier and 
less error-prone. Often we notice it’s a problem that whoever is tracking the game 
may lose track of how many buy-ins someone has and how much money each person is owed/are 
owed. In addition, we wanted to make a system that makes tracking these poker games more fun 
and more of a social event, hence making our online interface. 

The system should act as follows:
* Central display should get plugged into the wall/outlet.

    * Player modules will send location information about their location to the server. 

    * Users will then be prompted to enter a username using the brick phone keypad method:

        * Users can scroll through letters in a similar fashion to old cellphone keyboards. They will hit the # key to confirm a letter choice, and when they’re happy with the username they chose they will hit the * key to enter them into the game with a stack size of 200 chips.. 

    * After usernames have been entered, each player will input how much from their stack they inputted into the pot. Again, hitting the * key will send the values through.

    * The pot size message on the central display will also update accordingly. 

    * If someone wants to be buzzed into the building to join the game (by entering their name on the website “buzz-in” section), the central display message will change to who needs to be let in and where they are. Additionally, a buzzing tone will be heard until the request is resolved (via a button press). 


## System Diagram
<iframe src="https://drive.google.com/file/d/1gjcggRdOJEUGCVmnW1aFk2lsceLpVvmq/preview" width="640" height="480"></iframe>

The system is broken up into three different components: the Player Module, the Central Display, and the Camera Module.

* The Player Module is resopnsible for updating the webserver databases with user information such as the stack size, the name of the player and where the game is being hosted on campus. 

* The Central Display handles functionalities such as alerting players when someone needs to be let into the dorm, along with continually updating its large display with the current pot size

* The Camera Module takes pictures and sends the hex data up to the the webserver to be stored in a database.


## System Schematics
### RGB Matrix Display Pin Layout (ESP32 Central Display Module)
<iframe src="https://drive.google.com/file/d/1IO_N6P1bR3W56FiRDxjsspQOr7EXhwi7/preview" width="640" height="480"></iframe>

### RGB Matrix Plug Pin Layout
<iframe src="https://drive.google.com/file/d/1vO5TgZ0qUhdwg6yX1h-T3uiXVtZLM-fJ/preview" width="640" height="480"></iframe>

### Keypad Pin Layout (ESP32 Player Module)
<iframe src="https://drive.google.com/file/d/19LlasmlDsiy5JG4qD22oaX7k06XQfE1g/preview" width="640" height="480"></iframe>


## Parts List
<a href="https://www.adafruit.com/product/2277"> Large Adafruit LCD Display</a>
<ul>
    <li class="asterisk">Total price: $49.95</li>
    <li class="asterisk">This large LCD Display is used in the Display Module and serves as a nice big screen for players to see important informations such as the pot size.</li>
</ul>

<a href="https://www.amazon.com/gp/product/B00NAY2XUS/ref=as_li_tl?ie=UTF8&tag=zlufy-20&camp=1789&creative=9325&linkCode=as2&creativeASIN=B00NAY2XUS&linkId=1a55c92917477c650a3c4ce82cee6c82"> Adafruit Membrane 3x4 Matrix Keypad</a>
<ul>
    <li class="asterisk">Total price: $33.84</li>
    <li class="asterisk">The keyboard is used in the Player Module and is used to input user information such as their name or the amount they want to bet</li>
</ul>

<a href="https://www.amazon.com/Arducam-Module-Megapixels-Arduino-Mega2560/dp/B012UXNDOY"> Arducam Mini Module Camera</a>
<ul>
    <li class="asterisk">Total price: $25.99</li>
    <li class="asterisk">The Arducam is used in the Camera Module and is what allow users to record pictures that they want to cherish. </li>
</ul>

## Design Challenges and Decisions
* There was an issue with the ESP32 not having enough RAM, and thus we could not place the camera on the player module. As a result, we placed the camera on its own ESP32, which would then allow the camera to be a more mobile unit, and also avoided the RAM issue.

* Our project relied on multiple pieces of hardware that we had not used before in class: keypads, LED matrix, camera. As a result, there was a lot of time spent on figuring out how to use the hardware, before we could actually figure out how to integrate the hardware into our project.
    * The camera faced many issues of how to take pictures and how to upload image data to the server. Hex strings were very long, and thus we had to figure out how to send this data to the server. We decided to use very large buffer sizes that would allow the long strings of hex data to be sent via a single POST request to the server, rather than sending data in pieces across multiple POST requests. This made the process of storing the data in a database easier, as well as simplifying the process of making minor hex code edits for marker codes. 

* There were some code logistical challenges, such as: deciding how big stack sizes should be, how it would interact with the ledger, etc. We decided to default everyone’s stack size to the most common buy-in that our friends play: 20 dollar buy-in with 10 cents per white chip. This way we can more easily translate chips to cash on the ledger, with the ledger defaulting to 20-dollar buy-ins of 200 chips. This decision also allowed for a buy-in button on the keypad (“C”) that automatically buys a user back in with $20 (200 chips). In the future we may want to make this system more flexible so we can allow different chip values and stack sizes. 


## Server Side
### buzz.db
|user|location|claimed|timing
|---|---|---|---|
|Corey|Next House|1|2022-05-10 06:58:18.444960|
|Andrew|New House|1|2022-05-10 08:07:19.131789|
|Erin|New House|0|2022-05-10 08:17:32.957252|

When users fill out their username and current location on the website form to be let into the dorm,
this information gets saved into this database along with the claimed and timing tag. `claimed` is set to 
0 by default and is set to 1 when the person has been let in. 

### hex.db

|hexx|timing|
|---|---|
|fb39e174d837b...|2022-05-10 06:48:21.75214|

This table has two columns, `hexx` and `timing`, used to store captured images from the ESP. `hexx` stores the 
JPEG image data in hexadecimal, and `timing` stores the timestamp when the image was taken.

### ledger.db 
|username|net|
|---|---|
|CYN|-2000|
|C|-2000|
|K|-2000|
|D|-2000|
|M|-4000|
|A|-2000|

This table has two columns, `username` and `net`, used to record how much each player owes 
from buy-ins (in cents). For example, a user with a `net` of `-2000` owes `20` dollars.

### location.db

|location|timing|
|---|---|
|Next House|2022-05-10 07:52:05.218840|
|Off Campus|2022-04-26 08:16:14.016635|
|MacGregor House|2022-04-20 07:57:44.504945|

Upon starting a game, the ESP32 sends its coordinates to the webserver which is then logged on this database.
It then shows up on the landing page so that people interested in joining the game know where to go.

### leaderboard.db

|pot|timing|
|---|---|
|10|2022-05-10 05:08:39.518382|
|10|2022-05-10 05:08:39.522189|
|22|2022-05-10 05:09:24.543381|
|34|2022-05-10 05:09:24.546049|
|46|2022-05-10 05:09:34.559578|
|58|2022-05-10 05:09:34.562589|

This table keeps track of the current total pot size.

|username|stack|
|---|---|
|OACF|0|
|T|218|
|P|200|
|U|0|
|W|-12|
|cyn|60|
|john|142|

This table has two columns, `username` and `stack`, which stores how many chips are in the stacks of players.

# Detailed Code Layout

## ESP32 Player Module (player_module.ino)

This file’s purpose is to have users input their bet size and also the chips that they win during the game. It first prompts users to enter their username and then, as the game progresses, expects the user to input their bet sizes. This is the code that is responsible for keeping track of the players that are playing since each user will get their own player module. 

### enter_username()

This function is responsible for entering the username at the beginning of the game. The user will use an old cellphone keyboard method for entering their username, aka if they want to type a “B” they should press the “2” key twice. Once they are happy with their letter choice they should press the # key to confirm, and then once they are happy with the entire username they should press the * key to send it to the server and enter them into the game. 


### enter_pot()

This function reads the keypad input and waits for the user to hit the * key. If the * key is pressed then the code will send the current buffer (a number with the bet) to the server to be processed as a reduction or addition to their stack size. If they place a bet they should press the A key first, indicating that they are losing x amount from their stack. If they make a mistake they can hit the B key to clear the current buffer. If they hit the C key, it will indicate another buy-in. We have to keep track of the buy-ins to track how much everyone owes, because if someone buys in twice that shouldn’t be mistaken with someone earning twice their initial buy-in. 


### get_stack_for_user()

This function performs a GET request to the server by passing in a username. A value representing the specific username’s stack size gets returned back and is later displayed on the tft screen along with other user specific information like the username and location. 


### post_dorm_location()
This function performs a POST request to the server and sends through the body which dorm the current ESP32 setup is located. This would be where the game is taking place. 


## ESP32 Central Display Code (central_display_w_buzzer.ino)

This code’s primary function is to display the current pot size to ease bet sizing (typically players will want to bet ⅓ the pot size, but determining pot size requires counting chips). Additionally, when someone needs to be tapped into the building, it should change the display message to “[Insert name] is at [insert location]” as retrieved from the web server. A buzz tone will be heard as well. To resolve this message, a button at pin 45 needs to be pressed. The central display is also responsible for ending the game, by pressing a button at pin 39. 


### display_switch()
This function is responsible for alternating between the two display modes: pot size versus 
needing to let someone into the building. It accomplishes this by periodically pinging the 
python script that checks for entries in the database table. If it detects 
a new entry the display will switch to displaying the appropriate message in the 
form “[Insert name] is at [insert location”. 
* The display will switch back to pot size message once the button at pin 45 has been tapped, which is checked with the function tap_button_sm().
* This function will interact with buzz_handler.py

### clear_pot_button()

This function is responsible for alternating between the two display modes: pot size versus needing to let someone into the building. It accomplishes this by periodically pinging the python script that checks for entries in the database table. If it detects a new entry the display will switch to displaying the appropriate message in the form “[Insert name] is at [insert location]”. 
* The display will switch back to the pot size message once the button at pin 45 has been pressed, which is checked by the function tap_button_sm()
* This function will interact with buzz_handler.py

### end_game_sm()
This function is responsible for checking whether a button at pin 34 has been pressed, which indicates the end of a game. This function interacts with leaderboard.py

### pot_get_request() and pot_post_request()
Handle the requests for pot interactions. pot_get_request() retrieves the most recent pot size value by sending a GET request to pot.py, while pot_post_request() resets the pot size to 0 (after a hand) by sending a POST request to pot.py.

### clear_pot_button()
This function has a simple 3-state state machine to handle the button press that resets the pot at the end of a hand. When the button is pressed, pot_post_request() is called to reset the pot.

## ESP32 Camera Module (camera.ino)

This file’s purpose is to facilitate taking photos with the ArduCAM. This file is similar to the starter code, except that the web server on the ESP is removed. This is because instead of having the ArduCAM take a photo when the starter GUI sends a request to the ESP web server, we want the ArduCAM to take a photo when its corresponding button is pressed. Changes from the starter code also include a few lines that intercept hex data that is in the process of being written into the starter code’s default html file. There is also a button state machine that allows for photos to be taken upon the pressing of a button on the breadboard, rather than going through the starter code’s default html file.

### loop()
The loop function facilitates the button press that allows for a photo to be taken by calling the serverCapture() function. The photo data is then sent to the server via a POST request.


### make_post_request()
The function facilitates the POST request that sends hex data to hex.py on the server. It does so by writing the collected hex data into the body (which has a large buffer size) of the POST request.



## Webserver
The webserver serves as an interface for the ESP32 modules to interact with the databases in order to 
acheive a stateful system. It also serves as an area where users can see the leaderboard, notify the group
that they need to be let in, and see pictures. 


### update.py
The ESP32 Display Module periodically pings this endpoint to check for new updates within the buzzer_tables database.
If there are users in the database that still need to be let in, it returns a response that includes those users.

### coordinates.py
This code’s main function is to outline each of MIT’s undergraduate dorms using latitude and longitude coordinates. The within_area() function uses the Jordan Curve Theorem to determine whether or not a coordinate is within the polygon outline for a specific dorm. The coordinates outlining each dorm is stored in a dictionary called locations. 

### location_handler.py
This request handler function is responsible for keeping track of the players’ location. When the request handler receives a POST request, it adds the dorm name to location_table stored in location.db. If the request handler receives a GET request, then it would return the most recent dorm stored in the database table.

### pot.py
This file is responsible for keeping track of pot size and resetting the pot to zero following each hand. This is accomplished by returning the current pot size–performed by pot_query()–stored inside a database when the request handler receives a GET request (from central_display_w_buzzer.ino), and updating the pot size in the database to 0–performed by reset()–when the request handler receives a POST request.

### buzzer.html
This HTML page contains a form that allow users to submit their username and current location.
Upon submission, it sends a post request to main.py where the information is then stored in the buzzer_table.

### main.py
This code is responsible for updating the buzzer_table database. It records the username, location, timestamp, and 
a boolean that signifies whether or not they've been let it. 

### leaderboard.py
This file interacts with the database storing the stack sizes of the players. For GET requests, it takes in a parameter `leaderboard`, which is either `current` or `total` depending on if they want the leaderboard for the current game or the lifetime leaderboard, respectively, and a list will be returned, representing the leaderboard. If a `username` parameter is provided, only the specific user’s stack will be returned. For POST requests, the handler takes in parameters `username` and `delta` and changes the user’s stack by `delta`. In addition to handling player stack sizes, every bet a player places also updates the pot size in the database. It does so by calling the update_pot() function by first retrieving the current pot size from the database (via the pot_query() function) and then adding the newly placed bet (“delta”) to the current pot size. This value is then put into the pot size database.

### hex.py
This file is responsible for handling hex code for a photo taken by the camera. It takes the hex code from a POST request from camera.ino and adds this to a database of hex codes via the update_database() function. When hex.py receives a GET request, it retrieves all the hex codes in the database via the get_database() function and converts them to base 64. The data is then embedded into the `src` attributes of a list of `img` elements, which are returned to the browser.

### buzz_handler.py
This code is responsible for retrieving information from the database table that contains information 
about who needs to be tapped into the building. A get request to this file will scrape the database table 
for a user whose value is “1” meaning that they haven’t been let into the building yet. It also removes 
entries from the table older than a day. A post request to this file will indicate that a user has been 
let into the building and set that user’s value to 0 in the database table.

### ledger.py

This file is responsible for handling the ledger. A POST request can be sent to this endpoint with parameters `username` and `delta` to update how much they owe from buy-ins. A GET request can also be sent to this endpoint to return a GUI. Based on the players’ stack sizes and number of buy-ins, the GUI will show how much each player earns (or loses) if they were to leave the game. The GUI also has buttons to remove specific player entries.




# Weekly Demo and Milestone Videos

## Camera Functionality

### ArduCAM
<iframe width="1904" height="831" src="https://www.youtube.com/embed/H0NHjfOy9Y4" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### ArduCAM Part 2
<iframe src="https://drive.google.com/file/d/1zmbTuXrIvK5VQM--J_NkhlOugysKco3J/preview" width="640" height="480" allow="autoplay"></iframe>

### Camera Button
<iframe src="https://drive.google.com/file/d/1mUE-Tu7gnS7G2U1SuIm4HmBTXbuta8t9/preview" width="640" height="480" allow="autoplay"></iframe>

### ArduCAM Part 3
<iframe width="1904" height="831" src="https://www.youtube.com/embed/aRueEZsCdu0" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### Multiple Photos
<iframe src="https://drive.google.com/file/d/1nbxgqa2vuBkhGcPh2WBg70BC3UeUo2ss/preview" width="640" height="480" allow="autoplay"></iframe>



## ESP32 Player Module User Input and Data

### Brick Phone Keyboard
<iframe width="1904" height="831" src="https://www.youtube.com/embed/KS0cXMTyb6g" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### Keyboard
<iframe width="1904" height="831" src="https://www.youtube.com/embed/J-9HHsXa0WY" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### ESP32 Player- displaying individual user data on device
<iframe width="1904" height="831" src="https://www.youtube.com/embed/ZIReFh1xFrc" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>



## Buzzer Functionality 
### Server Side Buzzer
<iframe width="1904" height="831" src="https://www.youtube.com/embed/MO0PkGSpLXI" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### ESP Display Buzzer
<iframe src="https://drive.google.com/file/d/14gXCFTHK5yY6TS54eDsCAwOXtT3cnzb7/preview" width="640" height="480" allow="autoplay"></iframe>



## Location Tracker
### MIT Dorm Detector
<iframe width="1904" height="831" src="https://www.youtube.com/embed/9nWL6-FDyMg" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### GPS and Front-end HTML
<iframe width="1904" height="831" src="https://www.youtube.com/embed/TwUmYFhrWyU" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


## RGB Matrix Display
### RGB Array 
<iframe width="1904" height="831" src="https://www.youtube.com/embed/RjuysYsFwO4" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### RGB Array Part 2
<iframe width="1904" height="831" src="https://www.youtube.com/embed/bxm99XoKTCg" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### Integrating central system with buzzer
<iframe width="1904" height="831" src="https://www.youtube.com/embed/7XNpDhN0D0A" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


## Webserver Functionality
### Poker Database
<iframe src="https://drive.google.com/file/d/1igDgGm6H2rc3TfAp25_2wfMATKAOLDoB/preview" width="640" height="480" allow="autoplay"></iframe>

### Leaderboard
<iframe src="https://drive.google.com/file/d/1ko3pxdYjfHleWrRFIYyt1iIoC6mwcu-R/preview" width="640" height="480" allow="autoplay"></iframe>

### Webserver- Integrating all functionalities of the webserver
<iframe src="https://drive.google.com/file/d/1T2Le9vAqaA0WFvWlcL38GB6c4o0iM2IE/preview" width="640" height="480" allow="autoplay"></iframe>





# Team Members
- Monica Chan (mochan)
- Joy Lin (jlin)
- William Lin (tmwillin)
- Cynthia Zhang (zcynthia)
- Andrew Jiang (linjiang)
<!-- Markdeep: --><style class="fallback">body{visibility:hidden;white-space:pre;font-family:monospace}</style><script src="markdeep.min.js"></script><script src="https://casual-effects.com/markdeep/latest/markdeep.min.js?"></script><script>window.alreadyProcessedMarkdeep||(document.body.style.visibility="visible")</script>
