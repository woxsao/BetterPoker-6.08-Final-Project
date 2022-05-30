/* ----------------------------------------------------------------------
"Simple" Protomatter library example sketch (once you get past all
the various pin configurations at the top, and all the comments).
Shows basic use of Adafruit_Protomatter library with different devices.
This example is written for a 64x32 matrix but can be adapted to others.
Once the RGB matrix is initialized, most functions of the Adafruit_GFX
library are available for drawing -- code from other projects that use
LCDs or OLEDs can be easily adapted, or may be insightful for reference.
GFX library is documented here:
https://learn.adafruit.com/adafruit-gfx-graphics-library
------------------------------------------------------------------------- */

#include <Adafruit_Protomatter.h>
#include <string.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

/* ----------------------------------------------------------------------
The RGB matrix must be wired to VERY SPECIFIC pins, different for each
microcontroller board. This first section sets that up for a number of
supported boards. Notes have been moved to the bottom of the code.
------------------------------------------------------------------------- */

#if defined(_VARIANT_MATRIXPORTAL_M4_) // MatrixPortal M4
  uint8_t rgbPins[]  = {7, 8, 9, 10, 11, 12};
  uint8_t addrPins[] = {17, 18, 19, 20, 21};
  uint8_t clockPin   = 14;
  uint8_t latchPin   = 15;
  uint8_t oePin      = 16;

#elif defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) // Feather ESP32-S2 + RGB Matrix FeatherWing
  uint8_t rgbPins[]  = {6, 5, 9, 11, 10, 12};
  uint8_t addrPins[] = {A5, A4, A3, A2};
  uint8_t clockPin   = 13;
  uint8_t latchPin   = 0;
  uint8_t oePin      = 1;

#elif defined(ESP32)
  // 'Safe' pins, not overlapping any peripherals:
  // GPIO.out: 4, 12, 13, 14, 15, 21, 27, GPIO.out1: 32, 33
  // Peripheral-overlapping pins, sorted from 'most expendible':
  // 16, 17 (RX, TX)
  // 25, 26 (A0, A1)
  // 18, 5, 9 (MOSI, SCK, MISO)
  // 22, 23 (SCL, SDA)
  uint8_t rgbPins[]  = {4, 12, 13, 14, 15, 16};
  uint8_t addrPins[] = {7, 8, 9, 10};
  uint8_t clockPin   = 5; // Must be on same port as rgbPins
  uint8_t latchPin   = 33;
  uint8_t oePin      = 37;

#endif



Adafruit_Protomatter matrix(
  64,          // Width of matrix (or matrix chain) in pixels
  4,           // Bit depth, 1-6
  1, rgbPins,  // # of matrix chains, array of 6 RGB pins for each
  4, addrPins, // # of address pins (height is inferred), array of pins
  clockPin, latchPin, oePin, // Other matrix control pins
  false);      // No double-buffering here (see "doublebuffer" example)


// OTHER VARIABLES --------------------------------------
int16_t pot = 200;
int16_t textX = matrix.width();
int16_t textMin = 0; //change this when you have a string.
char* guest = "Cynthia";
char tap[1000];


#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

const uint16_t RESPONSE_TIMEOUT = 6000;
const uint16_t IN_BUFFER_SIZE = 4096; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 4096; //size of buffer to hold HTTP response
const uint16_t JSON_BODY_SIZE = 4096;
char request_buffer[IN_BUFFER_SIZE];
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
char username[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request
char location[OUT_BUFFER_SIZE];
char json_body[JSON_BODY_SIZE];
char body[100];
char default_username[] = "-";
//STATE MACHINE AND BUTTONS------------------------------------------------
const int tap_button = 45; //pin connected to button to acknowledge tap messages
const int pot_button = 39; // pin connected to button to deal with pot clearing
const uint8_t DISPLAY_POT = 0;
const uint8_t DISPLAY_GUEST = 1;
const uint8_t IDLE = 0;
const uint8_t PRESS = 1;
const uint8_t RELEASE = 2;
unsigned long timer;
int display_state = DISPLAY_POT;
int tap_button_state = IDLE;
//WIFI-----------------------------------------------------------------------
WiFiClient client2; //global WiFiClient Secure object
const char NETWORK[] = "MIT";
const char PASSWORD[] = "";
uint8_t channel = 1; //network channel on 2.4 GHz

// SETUP - RUNS ONCE AT PROGRAM START --------------------------------------

void setup(void) {
  Serial.begin(115200);

  // Initialize matrix...
  ProtomatterStatus status = matrix.begin();
  Serial.print("Protomatter begin() status: ");
  Serial.println((int)status);
  if(status != PROTOMATTER_OK) {
    // DO NOT CONTINUE if matrix setup encountered an error.
    for(;;);
  }
  WiFi.begin(NETWORK, PASSWORD, 1);
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

  pinMode(tap_button, INPUT_PULLUP);
  pinMode(pot_button, INPUT_PULLUP);
}

// LOOP - RUNS REPEATEDLY AFTER SETUP --------------------------------------

void loop(void) {
  // Since there's nothing more to be drawn, this loop() function just
  // shows the approximate refresh rate of the matrix at current settings.
  //tapIn();
  tap_sm();
}

void tap_sm(){
  switch(display_state){
    case DISPLAY_POT:
      potSize();
      tap_get_request();
      if(strcmp(username, default_username)!=0 && strlen(username)!=0){
        Serial.printf("STRCMP: %d \n", strcmp(username, "0"));
        Serial.printf("lenth: %d \n", strlen(username));
        Serial.printf("USERNAME: %s \n", username);
        Serial.printf("length default: %d \n", strlen(default_username));
        Serial.println(default_username);
        //delay(1000);
        display_state = DISPLAY_GUEST;
      }
      break;
    case DISPLAY_GUEST:
      //Serial.println("HERHERHEHREHRHERHEHREH");
      //Serial.printf("username %s \n", username);
      tapIn();
      timer = millis();
      tap_button_sm(digitalRead(tap_button));
      while(millis()-timer <=100){}
      break;
  }
}

void tap_button_sm(int button_state){
  switch(tap_button_state){
    case IDLE:
      if(button_state ==0)
        tap_button_state = PRESS;
      break;
    case PRESS:
      if(button_state ==1)
        tap_button_state = RELEASE;
      break;
    case RELEASE:
      tap_post_request();
      tap_button_state = IDLE;
      break;
  }
}
void potSize(){
  matrix.setTextColor(matrix.color565(34, 110, 55));
  matrix.setTextSize(1);
  matrix.setCursor(0,0);
  matrix.println("Pot Size:");
  matrix.printf("%d", pot);
  matrix.show();

}

void tapIn(){
  sprintf(tap, "%s is at %s", username, location);
  textMin = strlen(tap)*-6;
  //Serial.printf("min size: %d", textMin);
  //Serial.println();
  matrix.setTextWrap(false);
  matrix.fillScreen(0);
  matrix.setCursor(textX, 10);
  matrix.setTextColor(matrix.color565(121, 50, 168));
  matrix.print(F2(tap));
  matrix.show();
  textX-= 1;
  if(textX < textMin){
    textX = matrix.width();
  }
}
void tap_get_request(){
  Serial.println("SENDING REQUEST");
  memset(request_buffer, 0, sizeof(request_buffer));
  memset(response_buffer, 0, sizeof(response_buffer));
  sprintf(request_buffer, "GET https://608dev-2.net/sandbox/sc/team20/webserver/buzz_handler.py HTTP/1.1\r\n");
  strcat(request_buffer,"Host: 608dev-2.net\r\n");
  strcat(request_buffer,"\r\n"); //new line from header to body
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  Serial.println("--------------------------");

  DynamicJsonDocument doc(500);
  char* starting1 = strchr(response_buffer, '{');
  char* ending1 = strrchr(response_buffer, '}');
  *(ending1 + 1) = NULL;
  deserializeJson(doc, starting1);
  // Test if parsing succeeds.
  const char* temp_username = doc["username"];
  const char* temp_location = doc["location"];
  
  
  strcpy(username, temp_username);
  strcpy(location, temp_location);
  strip_newline(username);
  strip_newline(location);
  Serial.println(username);
  Serial.println(location);
  Serial.println("--------------------------");
}

void strip_newline(char* buffer){
  char* ptr;
  if( (ptr = strchr(buffer, '\n')) != NULL)
    *ptr = '\0';
}
void tap_post_request(){
  display_state = DISPLAY_POT;
  matrix.fillScreen(0);
  matrix.show();
  Serial.println("SENDING REQUEST");
  memset(body, 0, sizeof(body));
  sprintf(body, "\{\"username\": \"%s\"\}", username);
  int body_len = strlen(body);
  memset(request_buffer, 0, sizeof(request_buffer));
  memset(response_buffer, 0, sizeof(response_buffer));
  sprintf(request_buffer, "POST https://608dev-2.net/sandbox/sc/team20/webserver/buzz_handler.py HTTP/1.1\r\n");
  strcat(request_buffer,"Host: 608dev-2.net\r\n");
  strcat(request_buffer,"Content-Type: application/json\r\n");
  sprintf(request_buffer + strlen(request_buffer),"Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
  strcat(request_buffer,"\r\n"); //new line from header to body
  strcat(request_buffer,body); //body
  strcat(request_buffer,"\r\n"); //new line from header to body
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  Serial.println("--------------------------");
  Serial.println(response_buffer);
  Serial.println("--------------------------");
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

