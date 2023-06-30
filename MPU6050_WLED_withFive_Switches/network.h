const char* ssid = "yourSSID";
const char* password = "yourPASSWORD";

// Change the IP address and port number to match WLED Controller
String WLED_IP_ADDRESS = "http://10.0.0.9";
const int port = 80;

WiFiServer server1(port);

/*
  HttpClient server1 settings
  default custom static IP
  WiFi Settings for WiFi Manager
*/
char static_ip[16] = "10.0.0.23";
char static_gw[16] = "10.0.0.1";
char static_sn[16] = "255.255.255.0";
char static_dns[16] = "10.0.0.1";
