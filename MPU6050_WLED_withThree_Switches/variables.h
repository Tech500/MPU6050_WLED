/*
    Developed by William Lucid and OpenAI's, ChatGPT  03/29/2023 1800 EDT
    Edit for your configuration

*/

#define ledPin 2            // Change 2 to the LED pin you are using        // LED pin
#define data 4               // I²C SDA
#define clock 15              // I²C SCL
//#define wakeupInt ?           // External Interrupt to wake from Deep Sleep --To be determined
const int switch1Pin = 13;  // Switch 1 pin
const int switch2Pin = 14;  // Switch 2 pin
const int interruptPin = 23; //

unsigned long debounceDelay = 200;  // Debounce delay in ms

int interval1 = 100;   // Switch debounce delay
int interval2 = 500;   // Gemeral purpose delay
int interval3 = 2000;  // Send motion URL Motion Group 1
int interval4 = 2000;  // Send URL Random Group 2
int interval5 = 5000;  // Send URL Random Group 3
int interval6 = 8000;  // Send URL Random Group 4

/* 
  millis() delay

  static uint32_t previous = millis();

    if ((millis() - previous) > 500) {
       oldtime = millis();

       //do somwthing

    }
*/

const char* ssid = "yourSSID";
const char* password = "yourPSSWORD";

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
