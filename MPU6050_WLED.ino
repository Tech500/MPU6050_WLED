
/*
    Developed by William Lucid with an assist from OpenAI's ChatGPT 02/24/2023  Only partially finished; has not been added directly to running WLED, project 
    is a work-in-progress.  Will need a usermod to be added and compiled for WLED.  Sketch was developed to generate varialbles for effects, Intensity, and color 
    palette variables of WLED project.  WLED Project:  https://kno.wled.ge/  WLED runs on ESP8266 or ESP32.
    
    WLED "List of Effects and Palettes":  https://github.com/Aircoookie/WLED/wiki/List-of-effects-and-palettes 

    Plan is to use a wand/paddle with the MPU6050 attached; user could "wave" the wand/paddle effecting changes to strip led, WLED "effects," "intensity," and
    "color Palette."
    
*/

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <HTTPClient.h>
#endif

#include <LittleFS.h>
#include <FTPServer.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

const char* ssid = "R2D2";
const char* password = "sissy4357";

WiFiClient client;

Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_temp, *mpu_accel, *mpu_gyro;

// tell the FtpServer to use LittleFS
FTPServer ftpSrv(LittleFS);

// Change the IP address and port number to match your setup
String server = "http://10.0.0.9";
const int port = 80;

WiFiServer server1(port);

int flag = 1;
int count = 1;
int pass_value = 0;
int url_index = 0;
int palette = 0;
int start_palette = 0;
int httpCode;

// Define the FX names and URLs
String FX_names[] = {
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
    //Boucing Balls  --url_index 0

    "/win&A=128&CL=hFFA000&C2=h000000&FX=91&SX=128&IX=128&C1=128&C2=128&C3=128&FP=0",


    // Chase Rainbow  --url_index 1

    "/win&A=128&CL=hFFA000&C2=h000000&FX=30&SX=128&IX=128&C1=128&C2=128&C3=128&FP=0",


    // Fireworks  --url_index 2

    "/win&A=128&CL=hFFA000&C2=h000000&FX=42&SX=96&IX=192&C1=128&C2=128&C3=128&FP=11",

    // Boucing Balls  --url_index 3

    "/win&A=128&CL=hFFA000&C2=h000000&FX=91&SX=128&IX=128&C1=128&C2=128&C3=128&FP=0",


    // Chase Rainbow  --url_index 4

    "/win&A=128&CL=hFFA000&C2=h000000&FX=30&SX=128&IX=128&C1=128&C2=128&C3=128&FP=0"      
	
};

// &A  0 - 255 Controls master brightness
// &FX 0 - 255 Controls speed of the FX  
// &IX 0 - 255 Controls IX
// &FP 0 - 46  Conrols Color Palette

String FX_name;

int FX, IX, FP, seconds;

// Define functions to generate random values
long random_fx() {
   FX = random(101);
   return(FX);
}

long random_fp() {
  FP = random(46);
  return(FP);
}

long random_seconds() {
	seconds = random(10, 20);
	return(seconds);
}

void setup() {

  Serial.begin(115200);

#if defined(ESP8266)
  Wire.begin(4, 5);
#elif defined(ESP32)
  Wire.begin(21,22);
#endif

  bool fsok = LittleFS.begin();
	Serial.printf_P(PSTR("FS init: %s\n"), fsok ? PSTR("ok") : PSTR("fail!"));

	wifi_Start();

	// setup the ftp server with username and password
	// ports are defined in FTPCommon.h, default is
	//   21 for the control connection
	//   50009 for the data connection (passive mode by default)
	ftpSrv.begin(F("ftp"), F("ftp")); //username, password for ftp.  set ports in ESP8266FtpServer.h  (default 21, 50009 for PASV)

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
	mpu_temp->printSensorDetails();

	mpu_accel = mpu.getAccelerometerSensor();
	mpu_accel->printSensorDetails();

	mpu_gyro = mpu.getGyroSensor();
	mpu_gyro->printSensorDetails();

	LittleFS.begin();   

  // Open a "log.txt" for appended writing
  File log = LittleFS.open("/MPU6050log.txt", "a");

  if (!log)
  {
    Serial.println("file '/MPU6050log.txt' open failed");
  }
  
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
    log.print("  Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    log.print("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    log.print("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    log.print("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    log.print("+-16G");
    break;
  }

  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    log.print("  Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    log.print("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    log.print("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    log.print("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    log.print("+- 2000 deg/s");
    break;
  } 

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
    log.print("  Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    log.print("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    log.print("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    log.print("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    log.print("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    log.print("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    log.print("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    log.print("5 Hz");
    break;
  }
      
  log.println("");
  log.close(); 

  delay(5000); 

  server1.begin();

}

void loop() {
	
	  MPU6050();
    
    // Make sure to call handleFTP() frequently   
    ftpSrv.handleFTP();    
    
}

void MPU6050(){
       
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  mpu_temp->getEvent(&temp);
  mpu_accel->getEvent(&accel);
  mpu_gyro->getEvent(&gyro);

  float scaled = 0.07782219916379;  // Scaling factor 255/32767

  float scaled_x = accel.acceleration.x * scaled * 25000;  // 25000, 25000, and 6000 act as "feedback gain."
  float scaled_y = accel.acceleration.y * scaled * 25000;
  float scaled_z = accel.acceleration.z * scaled * 6000;  
  
  //Effect 
  int FX = map(scaled_x, 0, 32767, 0, 255);
      
  //Intensity
  int IX = map(scaled_y, 0, 32767, 0, 255);
      
  //Color Palette
  int FP = map(scaled_z, 0, 32767, 0, 255);

  //define MPU6050_Motion and no_Motion <<-----------------------------------------------------
  while(FX > 2) {  //MPU6050 "wand/paddle" is in use
      Motion();
      exit;
  }

  while(FX < 2){   //MPU6050 "wand/paddle" not in use
      noMotion();
      exit;
  }

}

void Motion(){

  sensors_event_t accel;

  float raw_x = accel.acceleration.x;
  float raw_y = accel.acceleration.y;
  float raw_z = accel.acceleration.z;

  // Open a "log.txt" for appended writing
  File log = LittleFS.open("/MPU6050log.txt", "a");

  if (!log){
    Serial.println("file '/MPU6050log.txt' open failed");
  }
      
  /* Log values */
  log.print("FX:  \t");
  log.print(abs(FX));
  log.print(",");
  log.print("  IX:  \t ");
  log.print(abs(IX));
  log.print(",");
  log.print("  Color Palette:  \t");
  log.print(FP);
  log.print("\t\t\t\t");
  log.print("Raw x:  \t");
  log.print(abs(raw_x));
  log.print(",");
  log.print("  Raw y:  \t");
  log.print(abs(raw_y));
  log.print(",");  
  log.print("  Raw z:  \t");
  log.print(abs(raw_z));
  log.print("\n");
  log.close();   
      
  /* Print out the values */
  Serial.print("FX:  \t");
  Serial.print(abs(FX));
  Serial.print(",");
  Serial.print("  IX:  \t");
  Serial.print(abs(IX));
  Serial.print(",");  
  Serial.print("  Color Palette:  \t");
  Serial.print(FP);
  Serial.print(",");
  Serial.print("\t\t\t Raw x:  \t");
  Serial.print(abs(raw_x));
  Serial.print(",");
  Serial.print("  Raw y:  \t");
  Serial.print(abs(raw_y));
  Serial.print(",");  
  Serial.print("  Raw z:  \t");
  Serial.print(abs(raw_z));
  Serial.print("\n");

  delay(500);
  
  flag = 3;

}

void noMotion(){

  String effect_name;

  //Use predefined url's
	if (pass_value % 5 == 0) {   //pass_value divided by 5 == 0, increase the divisor for less frequent custom URLs
      String url = server + urls[url_index];
      HTTPClient http;
      #if defined(ESP8266)
       WiFiClient client;
       if(http.begin(client, url)){
      #elif defined(ESP32)
       if(http.begin(url)){
      #endif
      httpCode = http.GET();
      seconds = random_seconds();
      delay(seconds);
      FP = random_fp();
      Serial.print("\nPass_value: " + String(pass_value) + "\n");
      Serial.print("Http Response: ");
      Serial.print(httpCode);
      Serial.print(" URL index: ");
      Serial.print(url_index);
      Serial.print(" Sleep: ");
      Serial.print(seconds);
      Serial.print(" Color Palette: ");
      Serial.println(FP);
      http.end();
      }
      
      url_index = url_index++;
      
	    //Reset url_index
      if( url_index == 5){
        url_index = 0;
      }
	  
	    pass_value++;
    }

  //Modify color palette only
	if (pass_value % 12 == 0) {
	
    for (palette = start_palette; palette < 46; palette++) {
		seconds = random_seconds();
		FP= palette;
		Serial.print("Pass_value: ");
		Serial.println(pass_value);
		Serial.print("Effect: Chase Rainbow+");
		
    effect_name = "Chase Rainbow+"; 
    //Modify just Color Palette
    String url = server + "/win"
      + "&A="		+ String(128)
      + "&CL="  + "hFFA000"
      + "&C2="	+ "h000000"
      + "&FX="  + String(30)
      + "&SX="	+ String(128)
      + "&IX="	+ String(128)
      + "&C1="	+ String(128)
      + "&C2="	+ String(128)
      + "&C3="	+ String(128)
      + "&FP="	+ String(FP);
		
    HTTPClient http;
		#if defined(ESP8266)
      WiFiClient client;
      if(http.begin(client, url)){
    #elif defined(ESP32)
      if(http.begin(url)){
    #endif
    Serial.println(url);
		int httpCode = http.GET();
		int seconds = random_seconds();
		delay(seconds * 1000);
		FP= random_fp();
		Serial.print("\nPass_value: " + String(pass_value) + "\n"); 
		Serial.print("Http Response: ");
		Serial.print(httpCode);
		Serial.print(" URL index: ");
		Serial.print(url_index);
		Serial.print(" Sleep: ");
		Serial.print(seconds);
		Serial.print(" Color Palette: ");
		Serial.println(FP);
		http.end();
    }

		url_index = url_index++;
		pass_value++;
    }
  }

	//Modify both effect and color palette
	if (pass_value % 5 != 0) {

		FX = random_fx();
		FP = random_fp();
		seconds = random_seconds();
			
    effect_name = "Chase Rainbow+";	
    //Modify Effect and Color Palette	
    String url = server + "/win"
      + "&A="		+ String(128)
      + "&CL="  + "hFFA000"
      + "&C2="	+ "h000000"
      + "&FX="  + String(FX)
      + "&SX="	+ String(128)
      + "&IX="	+ String(128)
      + "&C1="	+ String(128)
      + "&C2="	+ String(128)
      + "&C3="	+ String(128)
      + "&FP="	+ String(FP);

		HTTPClient http;
		#if defined(ESP8266)
      WiFiClient client;
      if(http.begin(client, url)){
    #elif defined(ESP32)
      if(http.begin(url)){
    #endif
    Serial.println(url);
		int httpCode = http.GET();
		delay(seconds);
		Serial.print("\nPass_value: " + String(pass_value) + "\n");
		Serial.print("Http Response: ");
		Serial.print(httpCode);
		Serial.print(" URL index: ");
		Serial.print(url_index);
		Serial.print(" Sleep: ");
		Serial.print(seconds);
		Serial.print(" Color Palette: ");
		Serial.println(FP);
		http.end();
    }
    
    url_index = url_index++;
		pass_value++;
  }

}

void wifi_Start()
{

  WiFi.mode(WIFI_STA);

  //Server settings
  #define ip {10,0,0,23}
  #define subnet {255,255,255,0}
  #define gateway {10,0,0,1}
  #define dns {10,0,0,1}

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

  if(WiFi.status() != WL_CONNECTED)
  {
    
    wifi_Start();
    
  }

  Serial.println("WiFi Connected");
  Serial.println("local ip");
  Serial.println(WiFi.localIP());

}
