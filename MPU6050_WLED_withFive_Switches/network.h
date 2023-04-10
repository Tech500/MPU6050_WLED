const char* ssid = "yourSSID";
const char* password = "yourPASSWORD";

// Change the IP address and port number to match WLED Controller
String WLED_IP_ADDRESS = "http://10.0.0.9";
const int port = 80;

WiFiServer server1(port);

//HTTPClient server1 settings
#define ip \
  { 10, 0, 0, 23 }
#define subnet \
  { 255, 255, 255, 0 }
#define gateway \
  { 10, 0, 0, 1 }
#define dns \
  { 10, 0, 0, 1 }
