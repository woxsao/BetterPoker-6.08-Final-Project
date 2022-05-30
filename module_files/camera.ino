#include <WiFi.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"

const uint16_t TEST_SIZE = 16000;
char test[TEST_SIZE];


const uint16_t IN_BUFFER_SIZE = 20000;
const uint16_t OUT_BUFFER_SIZE = 20000;
char request_buffer[IN_BUFFER_SIZE];
char response_buffer[OUT_BUFFER_SIZE];
const int RESPONSE_TIMEOUT = 6000;
const int POSTING_PERIOD = 6000;

#if !(defined ESP32)
#error Please select the ArduCAM ESP32 UNO board in the Tools/Board
#endif
//This demo can only work on OV2640_MINI_2MP or ARDUCAM_SHIELD_V2 platform.
#if !(defined(OV2640_MINI_2MP) || defined(OV5640_MINI_5MP_PLUS) || defined(OV5642_MINI_5MP_PLUS) \
      || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_BIT_ROTATION_FIXED) \
      || (defined(ARDUCAM_SHIELD_V2) && (defined(OV2640_CAM) || defined(OV5640_CAM) || defined(OV5642_CAM))))
#error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif

// set GPIO17 as the slave select :
const int CS = 34;
const int CAM_POWER_ON = 10;
#if defined(OV2640_MINI_2MP) || defined(OV2640_CAM)
ArduCAM myCAM(OV2640, CS);
#elif defined(OV5640_MINI_5MP_PLUS) || defined(OV5640_CAM)
ArduCAM myCAM(OV5640, CS);
#elif defined(OV5642_MINI_5MP_PLUS) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_BIT_ROTATION_FIXED) || (defined(OV5642_CAM))
ArduCAM myCAM(OV5642, CS);
#endif

//Station mode you should put your ssid and password
const char* ssid = "MIT";   // Put your SSID here
const char* password = "";  // Put your PASSWORD here

static const size_t bufferSize = 2048;
static uint8_t buffer[bufferSize] = { 0xFF };

void start_capture() {
  myCAM.clear_fifo_flag();
  myCAM.start_capture();
}

char stra[80];

void camCapture(ArduCAM myCAM) {
  uint32_t len = myCAM.read_fifo_length();
  if (len >= MAX_FIFO_SIZE)  //8M
  {
    Serial.println(F("Over size."));
  }
  if (len == 0)  //0 kb
  {
    Serial.println(F("Size is 0."));
  }
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();
  uint8_t temp = 0, temp_last = 0;
  int i = 0;
  bool is_header = false;
  while (len--) {
    temp_last = temp;
    temp = SPI.transfer(0x00);
    //Read JPEG data from FIFO
    if ((temp == 0xD9) && (temp_last == 0xFF))  //If find the end ,break while,
    {
      buffer[i++] = temp;  //save the last  0XD9
      //Write the remain bytes in the buffer
      is_header = false;
      i = 0;
      myCAM.CS_HIGH();
      break;
    }
    if (is_header == true) {
      //Write image data to buffer if not full
      if (i < bufferSize) {
        buffer[i++] = temp;
          
          memset(stra, 0, 80);
          sprintf(stra, "%02x", temp);
          strcat(test, stra);
      } else {
        //Write bufferSize bytes image data to file
        i = 0;
        buffer[i++] = temp;
          
          memset(stra, 0, 80);
          sprintf(stra, "%02x", temp);
          strcat(test, stra);
      }
    } else if ((temp == 0xD8) & (temp_last == 0xFF)) {
      is_header = true;
      buffer[i++] = temp_last;
      buffer[i++] = temp;
    }
  }
}

void serverCapture() {
  start_capture();
  Serial.println(F("CAM Capturing"));

  int total_time = 0;

  total_time = millis();
  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
    ;
  total_time = millis() - total_time;
  Serial.print(F("capture total_time used (in miliseconds):"));
  Serial.println(total_time, DEC);

  total_time = 0;

  Serial.println(F("CAM Capture Done."));
  total_time = millis();
  camCapture(myCAM);
  total_time = millis() - total_time;
  Serial.print(F("send total_time used (in miliseconds):"));
  Serial.println(total_time, DEC);
  //set the CS as an output:
  pinMode(CS, OUTPUT);
  pinMode(CAM_POWER_ON, OUTPUT);
  Serial.println(F("CAM send Done."));
}

const int LOOP_PERIOD = 10;
uint32_t primary_timer = 0;

const int BUTTON = 45;
int lastButton = 1;

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  
  uint8_t vid, pid;
  uint8_t temp;
  digitalWrite(CAM_POWER_ON, HIGH);
#if defined(__SAM3X8E__)
  Wire1.begin();
#else
  Wire.begin();
#endif
  Serial.begin(115200);
  Serial.println(F("ArduCAM Start!"));



  // initialize SPI:
  SPI.begin();
  SPI.setFrequency(4000000);  //4MHz

  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55) {
    Serial.println(F("SPI1 interface Error!"));
    while (1)
      ;
  }

  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55) {
    Serial.println(F("SPI1 interface Error!"));
    while (1)
      ;
  }
#if defined(OV2640_MINI_2MP) || defined(OV2640_CAM)
  //Check if the camera module type is OV2640
  myCAM.wrSensorReg8_8(0xff, 0x01);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
  if ((vid != 0x26) && ((pid != 0x41) || (pid != 0x42)))
    Serial.println(F("Can't find OV2640 module!"));
  else
    Serial.println(F("OV2640 detected."));
#elif defined(OV5640_MINI_5MP_PLUS) || defined(OV5640_CAM)
  //Check if the camera module type is OV5640
  myCAM.wrSensorReg16_8(0xff, 0x01);
  myCAM.rdSensorReg16_8(OV5640_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg16_8(OV5640_CHIPID_LOW, &pid);
  if ((vid != 0x56) || (pid != 0x40))
    Serial.println(F("Can't find OV5640 module!"));
  else
    Serial.println(F("OV5640 detected."));
#elif defined(OV5642_MINI_5MP_PLUS) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_BIT_ROTATION_FIXED) || (defined(OV5642_CAM))
  //Check if the camera module type is OV5642
  myCAM.wrSensorReg16_8(0xff, 0x01);
  myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
  if ((vid != 0x56) || (pid != 0x42)) {
    Serial.println(F("Can't find OV5642 module!"));
  } else
    Serial.println(F("OV5642 detected."));
#endif


  //Change to JPEG capture mode and initialize the OV2640 module
  myCAM.set_format(JPEG);
  myCAM.InitCAM();
#if defined(OV2640_MINI_2MP) || defined(OV2640_CAM)
  myCAM.OV2640_set_JPEG_size(OV2640_320x240);
#elif defined(OV5640_MINI_5MP_PLUS) || defined(OV5640_CAM)
  myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);  //VSYNC is active HIGH
  myCAM.OV5640_set_JPEG_size(OV5640_320x240);
#elif defined(OV5642_MINI_5MP_PLUS) || defined(OV5642_MINI_5MP) || defined(OV5642_MINI_5MP_BIT_ROTATION_FIXED) || (defined(OV5642_CAM))
  myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);  //VSYNC is active HIGH
  myCAM.OV5640_set_JPEG_size(OV5642_320x240);
#endif

  myCAM.clear_fifo_flag();
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F("WiFi connected"));
  Serial.println("");
  Serial.println(WiFi.localIP());
}

void loop() {
  int button = digitalRead(BUTTON);
  if (button && !lastButton)
    serverCapture();
  if (test[0] != 0){    
    make_post_request();
    memset(test,0,TEST_SIZE);
  }
  lastButton = button;

  while (millis() - primary_timer < LOOP_PERIOD);
  primary_timer += LOOP_PERIOD;
}



uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}

void do_http_request(const char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  WiFiClient client;                    //instantiate a client object
  if (client.connect(host, 80)) {       //try to connect to host on port 80
    if (serial) Serial.print(request);  //Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size);  //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) {  //while we remain connected read out data coming back
      client.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) {  //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);
    count = millis();
    while (client.available()) {  //read out remaining text (body of response)
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


char body[18000];  //for body
void make_post_request() {
  sprintf(body, "hexcode=%s", test);
  int body_len = strlen(body);  //calculate body length (for header reporting)
  sprintf(request_buffer, "POST http://608dev-2.net/sandbox/sc/team20/webserver/hex.py HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len);  //append string formatted to end of request buffer
  strcat(request_buffer, "\r\n");                                                        //new line from header to body
  strcat(request_buffer, body);                                                          //body
  strcat(request_buffer, "\r\n");                                                        //new line
  Serial.println(request_buffer);
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  Serial.println(response_buffer);  //viewable in Serial Terminal
}
