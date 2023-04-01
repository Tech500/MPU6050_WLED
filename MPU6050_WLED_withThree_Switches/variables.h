/*
    Developed by William Lucid and OpenAI's, ChatGPT  03/29/2023 1800 EDT
    Edit for your configuration

*/

#define ledPin 2            // Change 2 to the LED pin you are using        // LED pin
#define data 4               // I²C SDA
#define clock 15              // I²C SCL
#define SWLed 27               //Effects interrupt Statis "ON" = noMotion
//#define wakeupInt ?           // External Interrupt to wake from Deep Sleep --To be determined
const int switch1Pin = 13;  // Switch 1 pin
const int switch2Pin = 14;  // Switch 2 pin
const int interruptPin = 23; //

unsigned long debounceDelay = 50;  // Debounce delay in ms

int rdSeconds = 3000;   //Experiment with range of seconds up or down.

/*
  Leave this commented out.  
  Determines the amount of delay between sending of URLs.

  long random_seconds() {
  seconds = random(1, rdSecond); 
  return (seconds);
  }

*/

int interval1 = 100;   // Switch debounce delay
int interval2 = 500;   // Gemeral purpose delay
int interval3 = 10 * 1000;  // Send motion URL Motion Group 1
int interval4 = 6 * 1000;  // Send URL Random Group 2
int interval5 = 9 * 1000;  // Send URL Random Group 3
int interval6 = 5 * 1000;  // Send URL Random Group 4

// MPU6050 scaling "Gain" values.
// Gain values past versions:  xGain = 12000 (good), yGain 9000 (?), zGain = 1600 (??)
int xGain = 12000;
int yGain = 9000;
int zGain = 7500;

static uint32_t previous = millis();

const char* ssid = "R2D2";
const char* password = "sissy4357";

unsigned long period = 20 * 1000;  //20 seconds

// Change the IP address and port number to match WLED Controller
String WLED_IP_ADDRESS = "http://10.0.0.9";
const int port = 80;

WiFiServer server1(port);

//HTTPClient Server settings
#define ip \
  { 10, 0, 0, 23 }
#define subnet \
  { 255, 255, 255, 0 }
#define gateway \
  { 10, 0, 0, 1 }
#define dns \
  { 10, 0, 0, 1 }