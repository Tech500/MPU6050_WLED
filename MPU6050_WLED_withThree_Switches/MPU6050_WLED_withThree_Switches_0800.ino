/*

    "MPU6050_WLED_withTwo_Switches.ino  03/30/2023 0800 EDT"    

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
#include <FTPServer.h>  //https://github.com/dplasa/FTPClientWLED_IP_ADDRESS
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "variables.h"

#define BUTTON_PIN_BITMASK 0x200000000  // 2^13 in hex
#define GPIO_NUM13 13

uint32_t previous = 0;

/*
#define ledPin 2            // Change 2 to the LED pin you are using        // LED pin
const int switch1Pin = 13;  // Switch 1 pin
const int switch2Pin = 14;  // Switch 2 pin
const int interruptPin = 23;
*/

int ledState = LOW;                         // Current state of the LED
int switch1State = LOW;                     // Current state of switch 1
int switch2State = LOW;                     // Current state of switch 2
int switch1PrevState = LOW;                 // Previous state of switch 1
int switch2PrevState = LOW;                 // Previous state of switch 2
unsigned long switch1LastDebounceTime = 0;  // Last time switch 1 was toggled
unsigned long switch2LastDebounceTime = 0;  // Last time switch 2 was toggled

String myString1 = "  No Motion";
String myString2 = "  Motion";

// Define the interrupt handler function
void switchInterrupt() {
  // Toggle the LED when the switch is pressed
  digitalWrite(ledPin, !digitalRead(ledPin));
}


portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

int toogleSWITCH = 0;
int passes = 0;

void handleInterrupt() {
  portENTER_CRITICAL_ISR(&mux);
  toogleSWITCH = 1;
  portEXIT_CRITICAL_ISR(&mux);
}

WiFiClient client;

Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_temp, *mpu_accel, *mpu_gyro;

// tell the FtpWLED_IP_ADDRESS to use LittleFS
FTPServer ftpSrv(LittleFS);

bool isToggled = false;     // initial state of the toggle
bool isTouched = false;     // variable to keep track of touch state
bool isRGBRunning = false;  // variable to keep track of RGB effect state

float startMicros;

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

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); break;
    default: Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

bool switch2Active = false;

void setup() {

  Serial.begin(9600);

  while (!Serial) {};

  Serial.print("\n\n\nMPU6050_WLED_withTwo_Switches.ino  03/30/2023 0800 EDT");

  while(! Serial){};

  delay(1000);  

  pinMode(ledPin, OUTPUT);  // configure LED pin as output

#if defined(ESP8266)
  Wire.begin(4, 5);
#elif defined(ESP32)
  Wire.begin(data, clock);
#endif

  pinMode(ledPin, OUTPUT);
  pinMode(switch1Pin, INPUT_PULLUP);
  pinMode(switch2Pin, INPUT_PULLUP);
  pinMode(interruptPin, INPUT_PULLUP);

  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);

  switch2Active = false;

  bool fsok = LittleFS.begin(true);
  Serial.printf_P(PSTR("\n\nFS init: %s\n"), fsok ? PSTR("ok") : PSTR("fail!"));

  wifi_Start();

  // setup the ftp WLED_IP_ADDRESS with username and password
  // ports are defined in FTPCommon.h, default is
  //   21 for the control connection
  //   50009 for the data connection (passive mode by default)
  ftpSrv.begin(F("ftp"), F("ftp"));  //username, password for ftp.  set ports in ESP8266FtpWLED_IP_ADDRESS.h  (default 21, 50009 for PASV)

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

  WLED_IP_ADDRESS.begin();

 //Print the wakeup reason for ESP32
  print_wakeup_reason();

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, 0);  // 1 = High, 0 = Low
}

void loop() {

  uint32_t previous = millis();

  //  if ((millis()-previous) > 500) {
  //    previous = millis();
  //          //do something
  //  }
  
  for (int x = 1; x < 5000; x++) {
    ftpSrv.handleFTP();
  }
 
  if(toogleSWITCH == 1){ 
    Serial.println("Interrupt occured");
    toogleSWITCH = 0;
  }

  // Check if switch 1 has been toggled
  if (switch1State != switch1PrevState) {
    // If the switch is now pressed
    if (switch1State == LOW) {
      Serial.println("Switch 1a pressed");
      // Toggle the LED
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      // Disable switch 2 if LED is off
      if (ledState == LOW){
        switch2State = LOW;  // Switch 2 dusabled
        Serial.println("Going to Deep Sleep");
        goToDeepSleep();
      }     
    }
    // Update the previous state of the switch
    switch1PrevState = switch1State;
  }

  // Check if switch 1 has been toggled
  if (switch1State != switch1PrevState) {
    // If the switch is now pressed
    if (switch1State == HIGH) {
      Serial.println("Switch 1b pressed");
      // Toggle the LED
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      // Disable switch 2 if LED is off
      if (ledState == HIGH) {
      // Switch 2 enabled
        Serial.println("Live");
      }
    }
    // Update the previous state of the switch
    switch1PrevState = switch1State;
  } 

  // Check if switch 2 has been toggled
  if (ledState == HIGH && switch2State != switch2PrevState) {
    // If the switch is now pressed
    if (switch2State == HIGH) {
      Serial.println("Switch 2a pressed");
      // Call function 1 or function 2 depending on the current state of the LED
      if (ledState == HIGH) {
        isRGBRunning = true;
        for (int x = 1; x < 2; x++) {
          Serial.println("noMotion");
        }
        noMotion();
      }
    }
    // Update the previous state of the switch
    switch2PrevState = switch2State;
  }

  // Check if switch 2 has been toggled
  if (ledState == HIGH && switch2State == switch2PrevState) {
    // If the switch is now pressed
    if (switch2State == LOW) {
      Serial.println("Switch 2b pressed");
      // Call function 1 or function 2 depending on the current state of the LED
      if (ledState == HIGH) {
        for (int x = 1; x < 2; x++) {
          Serial.println("Motion");          
        }
        isRGBRunning = true;
        motion();
      }
    }
    // Update the previous state of the switch
    switch2PrevState = switch2State;
  }

  // Debounce switch 1
  if ((millis() - switch1LastDebounceTime) > debounceDelay) {
    switch1State = digitalRead(switch1Pin);
    switch1LastDebounceTime = millis();
  }

  // Debounce switch 2
  if ((millis() - switch2LastDebounceTime) > debounceDelay) {
    switch2State = digitalRead(switch2Pin);
    switch2LastDebounceTime = millis();
  }
}

void motion() {
    
  while(toogleSWITCH == 0){
  
    // Get effects, intensity, and color palette from Accelerometer/Gyroscope (MPU6050)
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;

    mpu_temp->getEvent(&temp);
    mpu_accel->getEvent(&accel);
    mpu_gyro->getEvent(&gyro);

    //float scaled = 0.07782219916379;  // Scaling factor 255/32767
    float scaled = 0.299782219916379;  // Scaling factor to get closer to goal ranges

    float scaled_x = accel.acceleration.x * scaled * 12000;  // 25000, 25000, and 6000 act as "feedback gain."
    float scaled_y = accel.acceleration.y * scaled * 9000;
    float scaled_z = accel.acceleration.z * scaled * 1600;

    //Effect
    int effects = map(scaled_x, 0, 32767, 0, 255);

    //Intensity
    int intensity = map(scaled_y, 0, 32767, 0, 255);

    //Color Palette
    int colors = map(scaled_z, 0, 32767, 0, 255);

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

    //Group 1 --Modify Effect, Intensity, and Color Palette Created by MPU6050
    String url = WLED_IP_ADDRESS + "/win"
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
    if ((millis()-previous) > interval3) {
      previous = millis();
      http.begin(url);
    }    
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.println(httpCode + myString2);
    }
    http.end();
  }  
}

void noMotion() {

   while(toogleSWITCH == 0){

    //Group 2 --Use predefined url's
    if (pass_value % 5 == 0) {  //pass_value divided by 5 == 0, increase the divisor for less frequent custom URLs
      String url = WLED_IP_ADDRESS + urls[url_index];
      HTTPClient http;
#if defined(ESP8266)
      WiFiClient client;
      http.begin(client, url);
#elif defined(ESP32)
      if ((millis()-previous) > interval4) {
      previous = millis();
      http.begin(url);
    }  
#endif
      httpCode = http.GET();
      Serial.println(httpCode + myString1);
      FP = random_fp();
      http.end();

      pass_value = pass_value++;

      //Reset url_index
      if (url_index == 5) {
        url_index = 0;
      }

      url_index = url_index++;
    }

    //Group 3 --Modify color palette only
    if (pass_value % 12 == 0) {
      for (palette = start_palette; palette < 46; palette++) {
        FP = random_fp();
        FP = palette;
        //Modify just Color Palette
        String url = WLED_IP_ADDRESS + "/win"
                     + "&A=" + String(128)
                     + "&CL=" + "hFFA000"
                     + "&C2=" + "h000000"
                     + "&FX=" + String(30)
                     + "&SX=" + String(128)
                     + "&IX=" + String(128)
                     + "&C1=" + String(128)
                     + "&C2=" + String(128)
                     + "&C3=" + String(128)
                     + "&FP=" + String(FP);

        HTTPClient http;
#if defined(ESP8266)
        WiFiClient client;
        if ((millis()-previous) > interval5) {
      previous = millis();
      http.begin(url);
    }  
#elif defined(ESP32)
        http.begin(url);
#endif
        int httpCode = http.GET();
        Serial.println(httpCode + myString1);
        http.end();
        
      }
      url_index = url_index++;
    }

    //Group 4 --Modify both Effect and Color Palette
    if (pass_value % 5 != 0) {
      FX = random_fx();
      FP = random_fp();

      String url = WLED_IP_ADDRESS + "/win"
                   + "&A=" + String(128)
                   + "&CL=" + "hFFA000"
                   + "&C2=" + "h000000"
                   + "&FX=" + String(FX)
                   + "&SX=" + String(128)
                   + "&IX=" + String(128)
                   + "&C1=" + String(128)
                   + "&C2=" + String(128)
                   + "&C3=" + String(128)
                   + "&FP=" + String(FP);

      HTTPClient http;
#if defined(ESP8266)
      WiFiClient client;
      if ((millis()-previous) > interval6) {
      previous = millis();
      http.begin(url);
    }  
#elif defined(ESP32)
      http.begin(url);
#endif
      int httpCode = http.GET();
      Serial.println(httpCode + myString1);
      http.end();
    
    }
    url_index = url_index++;
  }    
}

void goToDeepSleep() {

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

void ledsOFF() {

  //Turn off LED Strip
  String url = WLED_IP_ADDRESS + "/win&T=0";

  HTTPClient http;
#if defined(ESP8266)
  WiFiClient client;
  if (http.begin(client, url)) {
#elif defined(ESP32)
  if (http.begin(url)) {
#endif
    int httpCode = http.GET();
  }
  http.end();
}

void ledsON() {

  //Modify Effect, Intensity, and Color Palette	from MPU6050
  String url = WLED_IP_ADDRESS + "/win&T=1";

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