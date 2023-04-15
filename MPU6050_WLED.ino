#include <WiFi.h>
#include <FTPServer.h>  //https://github.com/dplasa/FTPClientServer
#include <LittleFS.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <esp_sleep.h>
#include "variables.h"

int touchValue = 0;

touch_pad_t touchPin;

//#define TOUCH_PIN T4
#define TOUCH_THRESHOLD 40
#define MAX_DEEP_SLEEP_TIME_US 60000000 // 60 seconds

uint64_t deepSleepTime = (uint64_t) (touchValue - TOUCH_THRESHOLD) * MAX_DEEP_SLEEP_TIME_US / (1024 - TOUCH_THRESHOLD);

#define TIME_TO_SLEEP  deepSleepTime

WiFiClient client;

// tell the FtpServer to use LittleFS
FTPServer ftpSrv(LittleFS);

Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_temp, *mpu_accel, *mpu_gyro;

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

void callMotion(){
  touchPin = esp_sleep_get_touchpad_wakeup_status();

  switch(touchPin)
  {
    case 0  : Serial.println("Touch detected on GPIO 4"); break;
    case 1  : Serial.println("Touch detected on GPIO 0"); break;
    case 2  : Serial.println("Touch detected on GPIO 2"); break;
    case 3  : Serial.println("Touch detected on GPIO 15"); break;
    case 4  : Serial.println("Touch detected on GPIO 13"); break;
    case 5  : Serial.println("Touch detected on GPIO 12"); break;
    case 6  : Serial.println("Touch detected on GPIO 14"); break;
    case 7  : Serial.println("Touch detected on GPIO 27"); break;
    case 8  : Serial.println("Touch detected on GPIO 33"); break;
    case 9  : Serial.println("Touch detected on GPIO 32"); break;
    default : Serial.println("Wakeup not by touchpad"); break;
  }
}

void callback(){
  //Placeholder
}

void setup() {

  Serial.begin(115200);
  while(! Serial){}
  
  Serial.print("\nMPU6050_WLED_with_Deep_Sleep");

  callMotion();
  

  Wire.begin(21, 22);

    //Setup interrupt on Touch Pad 4 (GPIO13)
  touchAttachInterrupt(T4, callback, TOUCH_THRESHOLD);

  wifi_Start();

  server1.begin();

  Serial.println("\nAdafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  bool fsok = LittleFS.begin();
  Serial.printf_P(PSTR("\n\nFS init: %s\n"), fsok ? PSTR("ok") : PSTR("fail!"));

  // setup the ftp server with username and password
  // ports are defined in FTPCommon.h, default is
  //   21 for the control connection
  //   50009 for the data connection (passive mode by default)
  ftpSrv.begin(F("ftp"), F("ftp"));  //username, password for ftp.  set ports in ESP8266FtpServer.h  (default 21, 50009 for PASV)

  Serial.println("MPU6050 Found!");

  mpu_temp = mpu.getTemperatureSensor();
  mpu_accel = mpu.getAccelerometerSensor();
  mpu_gyro = mpu.getGyroSensor();
  
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  // Go to deep sleep for the determined time
  Serial.print("\nGoing to Deep Sleep");
  esp_sleep_enable_touchpad_wakeup();
  esp_deep_sleep_start();
}

void loop() {
}
/*
  for (int x = 1; x < 5000; x++) {
    ftpSrv.handleFTP();
  }

  // Determine the deep sleep time based on the time the Chromawand was not touched
  if (deepSleepTime > MAX_DEEP_SLEEP_TIME_US) {
    deepSleepTime = MAX_DEEP_SLEEP_TIME_US;
  }     
}  
*/

void motion() {

  //Serial.print("\nTouch Sensor Value:  " + String(threshold) + " Motion Detected *********************************");

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

    delay(500);     
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
