/*

    "MPU6050_WLED_with_Deep_Sleep.ino"

    Developed by William Lucid with an assist from OpenAI's ChatGPT 03/10/2023 @ 01:00 EST.  Only partially finished; has not been added directly to running WLED, project 
    is a work-in-progress.  Will need a usermod to be added and compiled for WLED.  Sketch was developed to generate varialbles for effects, Intensity, and color 
    palette variables of WLED project.  WLED Project:  https://kno.wled.ge/  WLED runs on ESP8266 or ESP32.
    
    WLED "List of Effects and Palettes":  https://github.com/Aircoookie/WLED/wiki/List-of-effects-and-palettes 

    Plan is to use a wand/paddle with the MPU6050 attached; user could "wave" the wand/paddle effecting changes to strip led, WLED "effects," "intensity," and
    "color Palette."
	
    Created for use with Athom ESP32 Music Controller and ESP32  running this Sketch.  Both connect by WiFi; ESP32 sends URL Strinigs using HTTPClient.  
    Sketch is Interrupt driven.
    
    This file is used for testing and logging continous "waving" of "paddle" that has the MPU6050 attached; to get  MIN and MAX values of range.  So far; best range of 
    the goal 0-255 has been with the following settings:
    
    MPU6050 settings:  16 G, 250 degrees, 5 Hz    
    
    Named the "paddle/wand;" Chromawand.  Chromawand uses touch pin sensor to activate/de-activate motion function.  When de-activated; noMotion function generate 
    random FX, and FP variables for custom URLs to send via HTTPClient to the Athom ESP32 controller running WLED.
		  
*/

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#elif defined(ESP32)
#include "driver/adc.h"
#include <esp_wifi.h>
#include <esp_bt.h>
#include <esp_sleep.h>
#include <WiFi.h>
#include <HTTPClient.h>
#endif
#include <LittleFS.h>
#include <FTPServer.h>  //https://github.com/dplasa/FTPClientServer
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "variables.h"

WiFiClient client;

Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_temp, *mpu_accel, *mpu_gyro;

// tell the FtpServer to use LittleFS
FTPServer ftpSrv(LittleFS);

int threshold = 55;
int touch_sensor_value = touchRead(T4);

bool touchdetected = false;

void IRAM_ATTR gotTouch(){
 touchdetected = true;
}

float startMicros;

unsigned long previousMillis;

int flag = 1;
int effectsFlag;
int count = 1;
int pass_value = 0;
int motion_active = 0;
int url_index = 0;
int palette = 0;
int start_palette = 0;
int httpCode;
int effects, intensity, colors;

// Define the FX names and URLs
String effect_Names[] = {
  "Off",
  "Static",
  "Blink",
  "Breathe",
  "Sweep",
  "Ripple",
  "Sequence",
  "Fire2012",
  "Sinela",
  "Confetti",
  "Juggle",
  "BPM",
  "Atmosphere",
  "Larson Scanner",
  "Rainbow",
  "Rainbow Cycle",
  "Heat",
  "Fireworks",
  "Fire Flicker",
  "Gradient",
  "Meteor",
  "Morse",
  "Plasma",
  "Twinkle",
  "Halloween",
  "Christmas",
  "Candle",
  "Cylon",
  "Police",
  "Party",
  "Fire Truck",
  "State Police",
  "Color Wipe",
  "Scan",
  "Fade",
  "Theater Chase",
  "Theater Chase Rainbow",
  "Running Lights",
  "Twinkle Random",
  "Twinkle Fade",
  "Twinkle Fade Random",
  "Sparkle",
  "Flash Sparkle",
  "Hyper Sparkle",
  "Strobe",
  "Strobe Rainbow",
  "Multi Strobe",
  "Blink Rainbow",
  "Chase White",
  "Chase Color",
  "Chase Random",
  "Chase Rainbow",
  "Chase Flash",
  "Chase Flash Random",
  "Chase Rainbow White",
  "Rainbow Runner",
  "Scanner",
  "Dual Scanner",
  "Polar Lights",
  "Halloween Eyes",
  "Bouncing Balls",
  "Bouncing Colors",
  "Bouncing Rainbow",
  "Fire",
  "Graph",
  "Line",
  "Radar",
  "Solid",
  "Filling Up",
  "Filling Down",
  "Filling Up Down",
  "Alternating",
  "Waves",
  "Pacman",
  "Game Of Life",
  "Spooky",
  "Explosion",
  "Fireworks Random",
  "Stars",
  "Glitter",
  "Blinky",
  "Flicker",
  "Smart Larson",
  "Circus Combustus",
  "Tracer",
  "Thunderstorm",
  "Snowflakes",
  "Sinelon",
  "Breathing",
  "Pulsing",
  "Double Pulsing",
  "Triple Pulsing",
  "Christmas Tree",
  "Candy Cane",
  "Wreath",
  "Hanukkah",
  "Kwanzaa",
  "Heartbeat",
  "Bicolor Chase",
  "Bicolor Chase Rainbow",
  "Rainbow Runner Gradient",
  "Pacman Rainbow",
  "Blend",
  "Sine Blend",
  "Custom"
};

String urls[] = {
  //Boucing Balls
  "/win&A=128&CL=hFFA000&C2=h000000&FX=91&SX=128&IX=128&C1=128&C2=128&C3=128&FP=0",


  // Chase Rainbow
  "/win&A=128&CL=hFFA000&C2=h000000&FX=30&SX=128&IX=128&C1=128&C2=128&C3=128&FP=0",


  // Fireworks
  "/win&A=128&CL=hFFA000&C2=h000000&FX=42&SX=96&IX=192&C1=128&C2=128&C3=128&FP=11",

  // Boucing Balls
  "/win&A=128&CL=hFFA000&C2=h000000&FX=91&SX=128&IX=128&C1=128&C2=128&C3=128&FP=0",


  // Chase Rainbow
  "/win&A=128&CL=hFFA000&C2=h000000&FX=30&SX=128&IX=128&C1=128&C2=128&C3=128&FP=0"

};

// &A  0 - 255 Controls master brightness
// &FX 0 - 255 Controls speed of the FX
// &IX 0 - 255 Controls IX
// &FP 0 - 46  Conrols Color Palette

String FX_name;
String effect_name;

int FX, IX, FP, seconds;

// Define functions to generate random values
long random_fx() {
  FX = random(101);  // Fixed value do not change
  return (FX);
}

long random_fp() {
  FP = random(46);  // Fixed value do not change
  return (FP);
}

long random_seconds() {
  seconds = random(1, 10);  //Experiment with range of seconds
  return (seconds);
}

void setup() {

  Serial.begin(9600);

  while (!Serial) {};

  Serial.print("\n\n\nMPU6050_WLED_with_Deep_Sleep.ino");

  delay(2000);

#if defined(ESP8266)
  Wire.begin(4, 5);
#elif defined(ESP32)
  Wire.begin(21, 22);
#endif

  bool fsok = LittleFS.begin(true);
  Serial.printf_P(PSTR("\n\nFS init: %s\n"), fsok ? PSTR("ok") : PSTR("fail!"));

  wifi_Start();

  // setup the ftp server with username and password
  // ports are defined in FTPCommon.h, default is
  //   21 for the control connection
  //   50009 for the data connection (passive mode by default)
  ftpSrv.begin(F("ftp"), F("ftp"));  //username, password for ftp.  set ports in ESP8266FtpServer.h  (default 21, 50009 for PASV)

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu_temp = mpu.getTemperatureSensor();
  mpu_accel = mpu.getAccelerometerSensor();
  mpu_gyro = mpu.getGyroSensor();
  
  LittleFS.begin();

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  
  server1.begin();
  
  //Setup interrupt on Touch Pad 4 (GPIO13)
  touchAttachInterrupt(T4, gotTouch, threshold);

  //Configure Touchpad as wakeup source
  esp_sleep_enable_touchpad_wakeup();
}

void loop() {

  for (int x = 1; x < 5000; x++) {
    ftpSrv.handleFTP();
  }

  if(touchdetected){
    threshold = touchRead(T4);
    touchdetected = false;
    pass_value++;
    motion_active = 1;
    motion();
  }

  if(! touchdetected){
   
    Serial.println("");
    Serial.println("\nChromawand going into Deep Sleep");
    Serial.println("");
    ledsOFF();
    goToDeepSleep();
  }      
  
} 

void motion() {

  Serial.print("\nTouch Sensor Value:  " + String(threshold) + " Motion Detected *********************************");

  /* Get new sensor events with the readings */
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;

  mpu_temp->getEvent(&temp);
  mpu_accel->getEvent(&accel);
  mpu_gyro->getEvent(&gyro);

  //float scaled = 0.07782219916379;  // Scaling factor 255/32767
  float scaled = 0.299782219916379;  // Scaling factor to get closer to goal ranges

  float scaled_x = accel.acceleration.x  * scaled * 12000;  // 25000, 25000, and 6000 act as "feedback gain."
  float scaled_y = accel.acceleration.y  * scaled * 9000;
  float scaled_z = accel.acceleration.z  * scaled * 1600;

  //Effect
  int effects = map(scaled_x, 0, 32767, 0, 255);

  //Intensity
  int intensity = map(scaled_y, 0, 32767, 0, 255);

  //Color Palette
  int colors = map(scaled_z, 0, 32767, 0, 255);

  motion_active = 1;  //exit sleep delay

  LittleFS.begin();

  // Open a "log.txt" for appended writing
  File log = LittleFS.open("/MPU6050log.txt", FILE_APPEND);

   if (!log) {
    Serial.println("file '/MPU6050log.txt' open failed");
  }
  
  /* Log values */
  log.print("Effects:  \t");
  log.print(abs(effects));
  log.print(",");
  log.print("  Intensity:  \t ");
  log.print(abs(intensity));
  log.print(",");
  log.print("  Color Palette:  \t");
  log.print(abs(colors));
  log.println("");
  log.close();

  effect_name = "Chase Rainbow+";
  //Modify Effect, Intensity, and Color Palette	from MPU6050
  String url = server + "/win"
              + "&A=" + String(128)
              + "&CL=" + "hFFA000"
              + "&C2=" + "h000000"
              + "&FX=" + abs(effects)
              + "&SX=" + String(128)
              + "&IX=" + abs(intensity)
              + "&C1=" + String(128)
              + "&C2=" + String(128)
              + "&C3=" + String(128)
              + "&FP=" + abs(colors);

  HTTPClient http;
#if defined(ESP8266)
  WiFiClient client;
  if (http.begin(client, url)) {
#elif defined(ESP32)
  if (http.begin(url)) {
#endif
    int httpCode = http.GET();
    http.end();       
  }

  url_index = url_index++;

  motion_active = 0; 
}


void goToDeepSleep()
{

  Serial.flush();

  //Serial.println("Going to sleep...");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  btStop();

  adc_power_off();
  esp_wifi_stop();
  esp_bt_controller_disable();

  // Go to sleep! Zzzz
  esp_deep_sleep_start();
}

void ledsOFF(){

//Modify Effect, Intensity, and Color Palette	from MPU6050
  String url = server + "/win"
              + "&A=" + String(0)
              + "&CL=" + "hFFA000"
              + "&C2=" + "h000000"
              + "&FX=" + abs(effects)
              + "&SX=" + String(128)
              + "&IX=" + abs(intensity)
              + "&C1=" + String(128)
              + "&C2=" + String(128)
              + "&C3=" + String(128)
              + "&FP=" + abs(colors);

  HTTPClient http;
#if defined(ESP8266)
  WiFiClient client;
  if (http.begin(client, url)) {
#elif defined(ESP32)
  if (http.begin(url)) {
#endif
    int httpCode = http.GET();
    http.end();       
  }

  url_index = url_index++;

  motion_active = 0; 
}  

void wifi_Start() {

  WiFi.mode(WIFI_STA);

  //setting the static addresses in function "wifi_Start
  IPAddress ip;
  IPAddress gateway;
  IPAddress subnet;
  IPAddress dns;

  WiFi.begin(ssid, password);

  WiFi.config(ip, gateway, subnet, dns);

  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  Serial.printf("Connection result: %d\n", WiFi.waitForConnectResult());

  if (WiFi.status() != WL_CONNECTED) {

    wifi_Start();
  }

  Serial.println("WiFi Connected");
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
}
