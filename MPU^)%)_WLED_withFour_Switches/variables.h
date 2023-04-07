/*
    Developed by William Lucid and OpenAI's, ChatGPT  03/29/2023 1800 EDT
    Edit for your configuration

*/

#define ledPin 2            // Change 2 to the LED pin you are using        // LED pin
#define data 4               // I²C SDA
#define clock 15              // I²C SCL
#define SWLed 33               //Effects interrupt Status "ON" = motion
//#define wakeupInt ?           // External Interrupt to wake from Deep Sleep --To be determined
const int switch1Pin = 13;  // Switch 1 pin
const int switch2Pin = 14;  // Switch 2 pin
const int interruptPin = 23; // Switch 3 pin
//External wake up = 27         Switch 4 pin     --Leave commented out

unsigned long debounceDelay = 25;  // Debounce delay in ms 

#define GPIO36 36  //GPIO for 10K pot --maybe 5 Meg pot better (more delay resolution).

int potValue = 0;

//int rdSeconds = 10000;   //Experiment with range of seconds up or down.
int seconds = 0;

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

// MPU6050 scaling "Feedback Gain" values.
// Gain values of past versions:  xGain = 12000 (good), yGain 9000 (?), zGain = 1600 (??)
int xGain = 12000;
int yGain = 9000;
int zGain = 7500;

static uint32_t previous = millis();
unsigned long period = 20 * 1000;  //20 seconds
