#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include "PCF8574.h"

#include <WiFiUdp.h>
#include <TimeLib.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#include <ArduinoJson.h>
#include "FS.h"
#include "EspConfig.h"

#include "config.h"
#include "globals.h"
#include "globalFunctions.h"
#include "timeFunctions.h"
#include "webtools.h"
#include "webpages.h"
#include "update.h"






void setup()
{
  pinMode ( led, OUTPUT );
  pinMode ( resetPin, INPUT );
  
  digitalWrite ( led, 1 );

  Wire.begin(4,0);

  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(backlightPin, OUTPUT);
  
  analogWrite(backlightPin, 200);    
  
  Serial.begin(115200);
  Serial.println();

  Serial.println("");

 
  Serial.println(F("Booting...."));
  
  Serial.println(F("Mounting FS..."));
  if (!SPIFFS.begin()) {
    Serial.println(F("Failed to mount file system"));
    return;
  }
  config.init();
  Serial.println("******************************");
  //config.dbg();
  Serial.println("******************************");
  

  
  if (WiFi.SSID().length() > 0)
  {
    //WiFi config available
    WiFi.mode(WIFI_STA);
    WiFi.hostname(config.devicename);
    Serial.println(WiFi.hostname());
    Serial.print("Connecting to ");
    Serial.print(WiFi.SSID());
    WiFi.begin();
    if (!config.dhcp)
    {
      WiFi.config(config.ip, config.gw, config.sn, config.ns0, config.ns1);
    }    
    byte timeout = 0;
    while ((WiFi.status() == WL_DISCONNECTED) && (timeout < 50)) 
    {
      delay(250);
      Serial.print(".");
      timeout++;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println(F(" WiFi connected"));
      Serial.println(F("IP address: "));
      Serial.println(WiFi.localIP());
    } 
    else 
    {
      Serial.print(F(" Error: "));
      Serial.println(WiFi.status() );
    }
  } 
  else 
  {
    installMode = true;
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    //connection failed or no saved config. Start AP
    Serial.println(F("No WiFi connection starting Access Point."));
    
    WiFi.mode(WIFI_AP);
    
    WiFi.softAPConfig(ap_IP, ap_IP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(config.devicename, ap_Pass);
    Serial.println(F("Access point started"));
    WiFi.scanNetworks(true);
  } 
  else 
  {
    Udp.begin(recievePort);
    Serial.print("Local port: ");
    Serial.println(Udp.localPort());
    Serial.println("waiting for sync");
    setSyncProvider(getNtpTime);
    setSyncInterval(2);
    while (year() == 1970)
    {
      //wait until first time sync    
    }
    setSyncInterval(300);
    
    
    Serial.println(now());
    breakTime((now() + config.timezone + getDSTofset()),Time);
    //CurrentDay=Time.Day;

    otaUpdate();
  }

  

  
  initWebServer();
}

bool scrsaver = false;

void loop()
{
  if (now() != CurrentTime)
  {
    //New Seccond
    CurrentTime = now();
    breakTime((now() + config.timezone + getDSTofset()),Time);

    byte byte1;
    byte byte2;
    byte byte3;

    if (!scrsaver){
      
      byte1 = (Time.Hour / 10) << 4;
      byte1 = byte1 | (Time.Hour % 10);
    
      byte2 = (Time.Minute / 10) << 4;
      byte2 = byte2 | (Time.Minute % 10);
      
      byte3 = (Time.Second / 10) << 4;
      byte3 = byte3 | (Time.Second % 10);
    }
    else {
      
      byte1 = random(0, 10) << 4;
      byte1 = byte1 | random(0, 10);
    
      byte2 = random(0, 10) << 4;
      byte2 = byte2 | random(0, 10);
      
      byte3 = random(0, 10) << 4;
      byte3 = byte3 | random(0, 10);
      if (Time.Second % 60 == 20){
        scrsaver = false;
      }
    }
    
    ParOut.write(5, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, byte2); 
    shiftOut(dataPin, clockPin, MSBFIRST, byte2);     
    shiftOut(dataPin, clockPin, MSBFIRST, byte2); 
    shiftOut(dataPin, clockPin, MSBFIRST, byte1); 
    shiftOut(dataPin, clockPin, MSBFIRST, byte2); 
    shiftOut(dataPin, clockPin, MSBFIRST, byte3);       
        
    ParOut.write(5, HIGH);
      
    if (Time.Minute != CurrentMin){
      //New Minute!
      CurrentMin=Time.Minute;
      if (Time.Hour != CurrentHour){
        //New Hour!
        scrsaver = true;
        
        CurrentHour=Time.Hour;
        if (Time.Day != CurrentDay){
          //New Day!
          CurrentDay=Time.Day;
          if (Time.Month != CurrentMonth){
            //New Month!
            CurrentMonth=Time.Month;
            if (Time.Year != CurrentYear){
              //New Year!
              CurrentYear=Time.Year;
            }
          }
        }
      }    
    }
  }

  webServer.handleClient();

  checkReset();
}
