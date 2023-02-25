
/*
    Developed by William Lucid with an assist from OpenAI's ChatGPT 02/24/2023  Only partially finished; has not been added directly to running WLED, project 
    is a work-in-progress.  Will need a usermod to be added and compiled for WLED.  Sketch was developed to generate varialbles for effects, Intensity, and color 
    palette variables of WLED project.  WLED Project:  https://kno.wled.ge/  
    
    WLED "List of Effects and Palettes":  https://github.com/Aircoookie/WLED/wiki/List-of-effects-and-palettes 

    Plan is to use a wand/paddle with the MPU6050 attached; user could "wave" the wand/paddle effecting changes to strip led, WLED "effects," "intensity," and
    "color Palette."
    
*/

#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <FTPServer.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

const char * ssid = "SSID";
const char * password = "PASSWORD";

//Server settings
#define ipaddress {10,0,0,23}
#define subnet {255,255,255,0}
#define gateway {10,0,0,1}
#define dns {10,0,0,1}

Adafruit_MPU6050 mpu;
Adafruit_Sensor *mpu_temp, *mpu_accel, *mpu_gyro;

// tell the FtpServer to use LittleFS
FTPServer ftpSrv(LittleFS);

int flag = 1;
int count= 1;

void setup(void) {
	
	Serial.begin(9600);
	while (!Serial)
	delay(10); // will pause Zero, Leonardo, etc until serial console opens

	Wire.begin(4,5);

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

}

void loop() {

  if(flag == 1){    //Pass 1 only
    for(int x = 1;x < 51;x++){
      Serial.println(count);
      count = count + 1;
      logtoFile();
      flag = 2;
    } 
    flag = 3;    
  }
  
  if(flag == 2){   //Pass 2-40
    count = count + 1;
    Serial.println(count);
    logtoFile();    
  }

  if(flag == 3){  
      Serial.println("\nFTP is available\n"); 
      for(int x = 1;x<11;x++){
        Serial.println("\n\n");
        flag = 4;
      }
      exit;
  }

  // Make sure to call handleFTP() frequently
  ftpSrv.handleFTP();    
  
}

void logtoFile(){

  if(flag >= 1){
       
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

    float raw_x = accel.acceleration.x;
    float raw_y = accel.acceleration.y;
    float raw_z = accel.acceleration.z;
    
    int effect = map(scaled_x, 0, 32767, 0, 255);
    int intensity = map(scaled_y, 0, 32767, 0, 255);
    int color_palette = map(scaled_z, 0, 32767, 0, 255);

    // Open a "log.txt" for appended writing
    File log = LittleFS.open("/MPU6050log.txt", "a");

    if (!log){
      Serial.println("file '/MPU6050log.txt' open failed");
    }
        
    /* Log values */
    log.print("Effect:  \t");
    log.print(abs(effect));
    log.print(",");
    log.print("  Intensity:  \t ");
    log.print(abs(intensity));
    log.print(",");
    log.print("  Color Palette:  \t");
    log.print(abs(color_palette));
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
    Serial.print("Effect:  \t");
    Serial.print(abs(effect));
    Serial.print(",");
    Serial.print("  Intensity:  \t");
    Serial.print(abs(intensity));
    Serial.print(",");  
    Serial.print("  Color Palette:  \t");
    Serial.print(abs(color_palette));
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
  }  
  flag = 3;
}

void wifi_Start()
{

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
 
  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");

  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  //setting static addresses
  IPAddress ip;
  IPAddress gateway;
  IPAddress subnet;
  IPAddress dns;

  WiFi.config(ip, gateway, subnet, dns);
 
  
  Serial.println("network...");

  if(WiFi.status() != WL_CONNECTED)  // Wait for the Wi-Fi to connect
  {

    WiFi.waitForConnectResult();

  }

  while(WiFi.status() == WL_NO_SSID_AVAIL)
  {
     
    delay(10 * 1000);
    
    Serial.println("No SSID availible");
    
  }

  Serial.printf("\nConnection result: %d\n", WiFi.waitForConnectResult());
  Serial.print("IP Address:  ");
  Serial.println(WiFi.localIP());
  Serial.print("Signal strength: ");
  Serial.println(WiFi.RSSI());
  Serial.println("WiFi Connected");
 
}
