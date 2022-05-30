#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>


TFT_eSPI tft = TFT_eSPI();

const int pinCol[4]={5,6,7,8};     //pins of the differents columns
const int pinRow[4]={4,3,2,1};     //pins of the differents rows

const uint16_t RESPONSE_TIMEOUT = 6000;
const uint16_t IN_BUFFER_SIZE = 4096; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 4096; //size of buffer to hold HTTP response
const uint16_t JSON_BODY_SIZE = 4096;
char request[IN_BUFFER_SIZE];
char response[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
char body[IN_BUFFER_SIZE];
char json_body[JSON_BODY_SIZE];
char request_buffer[IN_BUFFER_SIZE];
char response_buffer[OUT_BUFFER_SIZE];
char* dorm_holder;
int stack_size = 0;
char* USER = "";
unsigned long loop_timer;
unsigned long press_timer;
bool scanning = false;
int trueCols;
int trueRows;
char old_output;
char current_output;
char user_char;
int click_count = 0;
char deltastr[20000];
bool username_entered = false;
bool just_entered_username = false;
bool before_first_bet = true;
bool pressed = false;
bool blank = true;
bool start_game = true;

char username[20];
char brick_keys[8][4] = {
  {'A', 'B', 'C', '2'},
  {'D', 'E', 'F', '3'},
  {'G', 'H', 'I', '4'},
  {'J', 'K', 'L', '5'},
  {'M', 'N', 'O', '6'},
  {'P', 'Q', 'R', 'S'},
  {'T', 'U', 'V', '8'},
  {'W', 'X', 'Y', 'Z'}
};

char keys[4][4]={                  //correspondence with the keys of the keypad
  {'*','7','4','1'},
  {'0','8','5','2'},
  {'#','9','6','3'},
  {'D','C','B','A'}
};

/* CONSTANTS */
//Prefix to POST request:
const char PREFIX[] = "{\"wifiAccessPoints\": ["; //beginning of json body
const char SUFFIX[] = "]}"; //suffix to POST request
const char API_KEY[] = "AIzaSyAQ9SzqkHhV-Gjv-71LohsypXUH447GWX8"; //don't change this and don't share this

const uint8_t BUTTON = 45;
const int MAX_APS = 15;

/* Global variables*/
uint8_t button_state; //used for containing button state and detecting edges
int old_button_state; //used for detecting button edges
uint32_t time_since_sample;      // used for microsecond timing

WiFiClientSecure client; //global WiFiClient Secure object
WiFiClient client2; //global WiFiClient Secure object

char dorm_body[500]; //for body
char char_body[100];

//choose one:

//const char NETWORK[] = "18_62";
//const char PASSWORD[] = "";
//
//
const char NETWORK[] = "MIT";
const char PASSWORD[] = "";
//
//const char NETWORK[] = "608_24G";
//const char PASSWORD[] = "608g2020";

/* Having network issues since there are 50 MIT and MIT_GUEST networks?. Do the following:
    When the access points are printed out at the start, find a particularly strong one that you're targeting.
    Let's say it is an MIT one and it has the following entry:
   . 4: MIT, Ch:1 (-51dBm)  4:95:E6:AE:DB:41
   Do the following...set the variable channel below to be the channel shown (1 in this example)
   and then copy the MAC address into the byte array below like shown.  Note the values are rendered in hexadecimal
   That is specified by putting a leading 0x in front of the number. We need to specify six pairs of hex values so:
   a 4 turns into a 0x04 (put a leading 0 if only one printed)
   a 95 becomes a 0x95, etc...
   see starting values below that match the example above. Change for your use:
   Finally where you connect to the network, comment out
     WiFi.begin(network, password);
   and uncomment out:
     WiFi.begin(network, password, channel, bssid);
   This will allow you target a specific router rather than a random one!
*/
uint8_t channel = 1; //network channel on 2.4 GHz
byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41}; //6 byte MAC address of AP you're targeting.



/*----------------------------------
   wifi_object_builder: generates a json-compatible entry for use with Google's geolocation API
   Arguments:
    * `char* object_string`: a char pointer to a location that can be used to build a c-string with a fully-contained JSON-compatible entry for one WiFi access point
    *  `uint32_t os_len`: a variable informing the function how much  space is available in the buffer
    * `uint8_t channel`: a value indicating the channel of WiFi operation (1 to 14)
    * `int signal_strength`: the value in dBm of the Access point
    * `uint8_t* mac_address`: a pointer to the six long array of `uint8_t` values that specifies the MAC address for the access point in question.

      Return value:
      the length of the object built. If not entry is written, 
*/
int wifi_object_builder(char* object_string, uint32_t os_len, uint8_t channel, int signal_strength, uint8_t* mac_address) {
  int buff;
  char* temp_char_arr = object_string;
  if (os_len >= 75){
  buff = sprintf(temp_char_arr, "{\"macAddress\": \"%02x", mac_address[0]);
  buff += sprintf(temp_char_arr + buff, ":%02x", mac_address[1]);
  buff += sprintf(temp_char_arr + buff, ":%02x", mac_address[2]);
  buff += sprintf(temp_char_arr + buff, ":%02x", mac_address[3]);
  buff += sprintf(temp_char_arr + buff, ":%02x", mac_address[4]);
  buff += sprintf(temp_char_arr + buff, ":%02x\",\"signalStrength\": %d,\"age\": 0,\"channel\": %d}", mac_address[5], signal_strength, channel);
  }
  
  int len = strlen(temp_char_arr);

  if (len > os_len){
    return 0;
  }else{
    sprintf(object_string, "%s",temp_char_arr);
    return len;
  }
}

char*  SERVER = "googleapis.com";  // Server URL

uint32_t timer;

void setup() {
  Serial.begin(115200);               // Set up serial port

  //SET UP SCREEN:
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size, change if you want
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_PINK, TFT_BLACK); //set color of font to hot pink foreground, black background

  //SET UP BUTTON:
  delay(100); //wait a bit (100 ms)
  pinMode(BUTTON, INPUT_PULLUP);

  for(int i = 0; i < 4; i++){
    pinMode(pinCol[i], INPUT_PULLUP);
    pinMode(pinRow[i], INPUT_PULLUP);
    digitalWrite(pinCol[i], LOW);
  }


  //PRINT OUT WIFI NETWORKS NEARBY
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      Serial.printf("%d: %s, Ch:%d (%ddBm) %s ", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
      uint8_t* cc = WiFi.BSSID(i);
      for (int k = 0; k < 6; k++) {
        Serial.print(*cc, HEX);
        if (k != 5) Serial.print(":");
        cc++;
      }
      Serial.println("");
    }
  }
  delay(100); //wait a bit (100 ms)

  //if using regular connection use line below:
  WiFi.begin(NETWORK, PASSWORD);
  //if using channel/mac specification for crowded bands use the following:
  //WiFi.begin(network, password, channel, bssid);

  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(NETWORK);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  timer = millis();
}

//main body of code
void loop() {
  if(start_game){
    http_start_game();
    start_game = false;
  }
  toggle_keypad_function();
  // retrieve_user_information();
}

void update_ledger(){
  memset(body, 0, sizeof(body));
  sprintf(body, "username=%s&delta=%s", username, deltastr);
  int body_len = strlen(body);
  memset(request_buffer, 0, sizeof(request_buffer));
  memset(response_buffer, 0, sizeof(response_buffer));
  sprintf(request_buffer, "POST https://608dev-2.net/sandbox/sc/team20/webserver/ledger.py HTTP/1.1\r\n");
  strcat(request_buffer,"Host: 608dev-2.net\r\n");
  strcat(request_buffer,"Content-Type:  application/x-www-form-urlencoded\r\n");
  sprintf(request_buffer + strlen(request_buffer),"Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
  strcat(request_buffer,"\r\n"); //new line from header to body
  strcat(request_buffer,body); //body
  strcat(request_buffer,"\r\n"); //new line from header to body
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  Serial.println("--------------------------");
  Serial.println(response_buffer);
  Serial.println("--------------------------");
  memset(deltastr, 0, sizeof(deltastr));
}
void toggle_keypad_function(){
  loop_timer = millis();
  loopInput();
  while(millis()-loop_timer < 400){}
}

void loopInput(){
  if(username_entered == false){
    enter_username();
  }
  else{
    enter_pot();
  }
}

void enter_pot(){
  readBoard();
  if(current_output == '*' && just_entered_username == false){
    http_enter_username();
   //update_ledger();    
    memset(deltastr, 0, sizeof(deltastr));    
  }
  else if(current_output!= '*' && pressed == true){
    if(current_output == 'A'){
      current_output = '-';
    }
    if(current_output == 'B'){
      memset(deltastr, 0, sizeof(deltastr));
      current_output = '\0';
      tft.fillScreen(TFT_BLACK);
    }
    if(current_output == 'C'){
      strcpy(deltastr, "-2000");
      update_ledger();
      strcpy(deltastr, "200");
      http_enter_username();
      memset(deltastr, 0, sizeof(deltastr));
      current_output = '\0';
      
    }
    else if (current_output != '\0'){
      char str_char[2] = {current_output, '\0'};
      Serial.println("HERHERHEHREHRHE2");
      Serial.println(current_output);
      strcat(deltastr, str_char);
      press_timer = millis();
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0,0,2);
      tft.println(deltastr);
      pressed = false;
    }
  }
  if(current_output != '*'){
    just_entered_username = false;
  }    
}
void http_start_game(){
  Serial.println("SENDING REQUEST");
  strcpy(body, "game=start");
  Serial.println(body);
  int body_len = strlen(body);
  memset(request_buffer, 0, sizeof(request_buffer));
  memset(response_buffer, 0, sizeof(response_buffer));
  sprintf(request_buffer, "POST https://608dev-2.net/sandbox/sc/team20/webserver/leaderboard.py HTTP/1.1\r\n");
  strcat(request_buffer,"Host: 608dev-2.net\r\n");
  strcat(request_buffer,"Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request_buffer + strlen(request_buffer),"Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
  strcat(request_buffer,"\r\n"); //new line from header to body
  strcat(request_buffer,body); //body
  strcat(request_buffer,"\r\n"); //new line from header to body
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  Serial.println(response_buffer);
  current_output = '\0';
  tft.fillScreen(TFT_BLACK);
}
void enter_username(){
  if(strlen(username) == 0 && current_output == '\0'){
    tft.setCursor(0,0);
    tft.println("Please enter a username");
  }
  else if(blank){
    tft.fillScreen(TFT_BLACK);
    blank = false;
  }    
  readBoard();
  if(current_output == '#'){
    click_count = 0;
    char str_char[2] = {user_char, '\0'};
    strcat(username, str_char);
    current_output = '\0';
    user_char = '\0';    
  }
  else if(current_output == '*'){
    char str_char[2] = {'0', '\0'};
    char* initial_buyin = "200";
    //strcat(deltastr,str_char);
    strcpy(deltastr, initial_buyin);
    Serial.println(deltastr);  
    username_entered = true;  
    
    USER = username;
    sprintf(char_body, "username=%s&delta=%s", username, deltastr);
    Serial.println(char_body);
    http_enter_username();
    memset(deltastr, 0, sizeof(deltastr));
    just_entered_username = true;
    tft.fillScreen(TFT_BLACK);
    strcpy(deltastr, "-2000");
    update_ledger();
    pot_post_request();
    press_timer = millis();     
  }
  else if(current_output != '\0'){
    brick_keyboard();
  }
    tft.setTextColor(TFT_PINK, TFT_BLACK);
    tft.setCursor(0,0,2);
    tft.print(username);
  if(user_char!= '\0'){
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    Serial.println("HERE3");
    tft.println(user_char);    
  }
}
void readBoard(){
  for (int r=0; r<4; r++) {
    pinMode(pinRow[r], OUTPUT);
    for(int c=0; c<4; c++){
      if (digitalRead(pinCol[c]) == LOW) {
        pressed = true;
        old_output = current_output;
        current_output = keys[r][c]; //we dispalay the right symbol
        if(old_output == current_output){
          click_count += 1;
          if(click_count > 4)
            click_count %= 4;
        }
        else{
          click_count = 1;
        }
      }
    }
    pinMode(pinRow[r], INPUT_PULLUP);
  }
}

void brick_keyboard(){
  int num = current_output -'0';
  user_char = brick_keys[num -2][click_count-1];
}

void http_enter_username(){
  Serial.println("SENDING REQUEST");
  sprintf(body, "username=%s&delta=%s", username, deltastr);
  Serial.println(body);
  int body_len = strlen(body);
  memset(request_buffer, 0, sizeof(request_buffer));
  memset(response_buffer, 0, sizeof(response_buffer));
  sprintf(request_buffer, "POST https://608dev-2.net/sandbox/sc/team20/webserver/leaderboard.py HTTP/1.1\r\n");
  strcat(request_buffer,"Host: 608dev-2.net\r\n");
  strcat(request_buffer,"Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request_buffer + strlen(request_buffer),"Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
  strcat(request_buffer,"\r\n"); //new line from header to body
  strcat(request_buffer,body); //body
  strcat(request_buffer,"\r\n"); //new line from header to body
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  Serial.println(response_buffer);
  just_entered_username = true;
  current_output = '\0';
  tft.fillScreen(TFT_BLACK);
  retrieve_user_information();
}

void retrieve_user_information(){
    int offset = sprintf(json_body, "%s", PREFIX);
    int n = WiFi.scanNetworks(); //run a new scan. could also modify to use original scan from setup so quicker (though older info)
    Serial.println("scan done");
    if (n == 0) {
      Serial.println("no networks found");
    } else {
      int max_aps = max(min(MAX_APS, n), 1);
      for (int i = 0; i < max_aps; ++i) { //for each valid access point
        uint8_t* mac = WiFi.BSSID(i); //get the MAC Address
        offset += wifi_object_builder(json_body + offset, JSON_BODY_SIZE-offset, WiFi.channel(i), WiFi.RSSI(i), WiFi.BSSID(i)); //generate the query
        if(i!=max_aps-1){
          offset +=sprintf(json_body+offset,",");//add comma between entries except trailing.
        }
      }
      sprintf(json_body + offset, "%s", SUFFIX);
      Serial.println(json_body);
      int len = strlen(json_body);
      // Make a HTTP request:
      Serial.println("SENDING REQUEST");
      request[0] = '\0'; //set 0th byte to null
      offset = 0; //reset offset variable for sprintf-ing
      offset += sprintf(request + offset, "POST https://www.googleapis.com/geolocation/v1/geolocate?key=%s  HTTP/1.1\r\n", API_KEY);
      offset += sprintf(request + offset, "Host: googleapis.com\r\n");
      offset += sprintf(request + offset, "Content-Type: application/json\r\n");
      offset += sprintf(request + offset, "cache-control: no-cache\r\n");
      offset += sprintf(request + offset, "Content-Length: %d\r\n\r\n", len);
      offset += sprintf(request + offset, "%s\r\n", json_body);
      do_https_request(SERVER, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
      Serial.println("-----------");
      Serial.println(response);
      Serial.println("-----------");
      //For Part Two of Lab04B, you should start working here. Create a DynamicJsonDoc of a reasonable size (few hundred bytes at least...)
      DynamicJsonDocument doc(500);
      char* response_holder = strchr(response, '{');
      char *pch = strrchr(response_holder, '}');
      *(pch+1)= '\0';
      DeserializationError error = deserializeJson(doc, response_holder);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      double latitude = doc["location"]["lat"];
      double longitude = doc["location"]["lng"];
      Serial.println(latitude,6);
      Serial.println(longitude,6);

      // tester coordinates for MacGregor House:
      // latitude = 42.35537759968949;
      // longitude = -71.09983008636257;  

      get_stack_for_user();
          

      sprintf(request_buffer, "GET http://608dev-2.net/sandbox/sc/team20/webserver/coordinates.py?lon=%f&lat=%f HTTP/1.1\r\n",longitude,latitude);
      strcat(request_buffer, "Host: 608dev-2.net\r\n"); //add more to the end
      strcat(request_buffer, "\r\n"); //add blank line!
      //submit to function that performs GET.  It will return output using response_buffer char array
      do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
      Serial.println("\r\n"); //print to serial monitor
      Serial.println(response_buffer); //print to serial monitor
      // tft.printf("lat: %f\r\n", latitude);
      // tft.printf("lng: %f\r\n", longitude);
      response_buffer[strlen(response_buffer)-1] = '\0';
      dorm_holder = response_buffer;
      
      tft.fillScreen(TFT_BLACK); //black out TFT Screen
      tft.setCursor(0, 0, 1);
      tft.printf("Player Information: \r\n");
      tft.printf("username: %s\r\n", USER);
      tft.printf("place: %s\r\n", dorm_holder);
      tft.printf("stack size: %i\r\n", stack_size);
      //response_buffer[strlen(response_buffer)-1] = '\0';
      post_dorm_location(response_buffer);
    }
}
void pot_post_request(){ //clears pot at the end of each hand.
  sprintf(request_buffer, "POST http://608dev-2.net/sandbox/sc/team20/webserver/pot.py HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "\r\n");                                                        //new line from header to body
  Serial.println(request_buffer);
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  Serial.println(response_buffer);  //viewable in Serial Terminal

}
void get_stack_for_user(){
  sprintf(request_buffer, "GET http://608dev-2.net/sandbox/sc/team20/webserver/leaderboard.py?leaderboard=current&username=%s HTTP/1.1\r\n",USER);
  strcat(request_buffer, "Host: 608dev-2.net\r\n"); //add more to the end
  strcat(request_buffer, "\r\n"); //add blank line!
  //submit to function that performs GET.  It will return output using response_buffer char array
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  Serial.println("\r\n"); //print to serial monitor
  response_buffer[strlen(response_buffer)-1] = '\0';
  // Serial.println(response_buffer); //print to serial monitor
  Serial.println(strlen(response_buffer));
  stack_size = atoi(response_buffer);
  // Serial.println(stack_size);
}

void post_dorm_location(char* dorm){
  
  sprintf(dorm_body,"dorm_location=%s", dorm);
  int body_len = strlen(dorm_body); //calculate body length (for header reporting)
  sprintf(request_buffer,"POST http://608dev-2.net/sandbox/sc/team20/webserver/location_handler.py HTTP/1.1\r\n");
  strcat(request_buffer,"Host: 608dev-2.net\r\n");
  strcat(request_buffer,"Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request_buffer+strlen(request_buffer),"Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
  strcat(request_buffer,"\r\n"); //new line from header to body
  strcat(request_buffer,dorm_body); //body
  strcat(request_buffer,"\r\n"); //new line
  Serial.println(request_buffer);
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
  Serial.println(response_buffer); //viewable in Serial Terminal
}

void do_http_GET(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) { //found a blank line! (end of response header)
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);  //empty in prep to store body
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response, client.read(), OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}

//support functions below:

/*https communication requires certificates to be known ahead of time so each entity will trust one another.
   This is a cert for google's generic servers below, allowing us to connect over https with their resources
*/
const char* CA_CERT = \
                      "-----BEGIN CERTIFICATE-----\n" \
                      "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\n" \
                      "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\n" \
                      "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\n" \
                      "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\n" \
                      "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\n" \
                      "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\n" \
                      "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\n" \
                      "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\n" \
                      "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\n" \
                      "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\n" \
                      "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\n" \
                      "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\n" \
                      "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\n" \
                      "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\n" \
                      "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\n" \
                      "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\n" \
                      "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\n" \
                      "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\n" \
                      "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\n" \
                      "-----END CERTIFICATE-----\n";


/*----------------------------------
   char_append Function:
   Arguments:
      char* buff: pointer to character array which we will append a
      char c:
      uint16_t buff_size: size of buffer buff

   Return value:
      boolean: True if character appended, False if not appended (indicating buffer full)
*/
uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}



/*----------------------------------
 * do_http_request Function:
 * Arguments:
 *    char* host: null-terminated char-array containing host to connect to
 *    char* request: null-terminated char-arry containing properly formatted HTTP request
 *    char* response: char-array used as output for function to contain response
 *    uint16_t response_size: size of response buffer (in bytes)
 *    uint16_t response_timeout: duration we'll wait (in ms) for a response from server
 *    uint8_t serial: used for printing debug information to terminal (true prints, false doesn't)
 * Return value:
 *    void (none)
 */
void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial){
  if (client2.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client2.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client2.connected()) { //while we remain connected read out data coming back
      client2.readBytesUntil('\n',response,response_size);
      if (serial) Serial.println(response);
      if (strcmp(response,"\r")==0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis()-count>response_timeout) break;
    }
    memset(response, 0, response_size);  
    count = millis();
    while (client2.available()) { //read out remaining text (body of response)
      char_append(response,client2.read(),OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client2.stop();
    if (serial) Serial.println("-----------");  
  }else{
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client2.stop();
  }
}  

/*----------------------------------
   do_https_request Function. Similar to do_http_request, but supports https requests
   Arguments:
      char* host: null-terminated char-array containing host to connect to
      char* request: null-terminated char-arry containing properly formatted HTTP request
      char* response: char-array used as output for function to contain response
      uint16_t response_size: size of response buffer (in bytes)
      uint16_t response_timeout: duration we'll wait (in ms) for a response from server
      uint8_t serial: used for printing debug information to terminal (true prints, false doesn't)
   Return value:
      void (none)
*/
void do_https_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  client.setHandshakeTimeout(30);
  client.setCACert(CA_CERT); //set cert for https
  if (client.connect(host,443,4000)) { //try to connect to host on port 443
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    response[0] = '\0';
    //memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response, client.read(), OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}
