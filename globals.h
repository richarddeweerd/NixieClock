ESP8266WebServer webServer(80);
WiFiUDP Udp;

bool installMode = false;

time_t CurrentTime = 0;
TimeElements Time;

byte CurrentMin;
byte CurrentHour;
byte CurrentDay;
byte CurrentMonth;
byte CurrentYear;


PCF8574 ParOut(0x20);
