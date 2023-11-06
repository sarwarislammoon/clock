#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>               
#include <TimeLib.h>                 
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ST7796S_kbv.h"
#include <Fonts/FreeSansBold24pt7b.h>
#include "FreeSansBold60pt7b.h"
#include "FreeSansBold90pt7b.h"
#include <ESP32Time.h>


#define TFT_RST                 22
#define TFT_SCK                 14
#define TFT_DC                  21
#define TFT_CS                  15
#define TFT_MOSI                13
#define TFT_MISO                -1
#define TFT_BCKL                23



const char *ssid     =         "illuminati";
const char *password =         "1181996a";
 
WiFiUDP ntpUDP;
 
 
NTPClient timeClient(ntpUDP);
Adafruit_ST7796S_kbv tft = Adafruit_ST7796S_kbv(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);
ESP32Time rtc(0);

//variables 
char Time[ ] = "--:--";
String _date, _ampm, _last_date,  lastAmPm; 
byte last_second, second_, minute_, hour_, day_, month_, last_min, last_hour, last_val;
int year_;


//flags
bool isSecChange = true;
bool isMinuteChange = true;
bool isHourChange = true;
bool updateRTC= true;
bool isDateChange = true;
bool isAmPmChange =true;

void setup(){
 
  Serial.begin(115200);
  
  //Turn on back light
  pinMode(TFT_BCKL, OUTPUT);
  digitalWrite(TFT_BCKL, HIGH);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ST7796S_BLACK);
  

  // Try to connect wifi 
  WiFi.begin(ssid, password);
  Serial.print("Connecting.");
 
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected");
  

  //Start timeclient 
  timeClient.begin();
    
}
 

 void loop() {
 calculateDateTime();
 showDateTime();
}


// function to calculete date time 
void calculateDateTime(){
   if (updateRTC= true){
     timeClient.update();
     rtc.setTime(timeClient.getEpochTime());
     rtc.offset = -3600*7; //-3600*7
     updateRTC= false;
   } 
   
  //unsigned long unix_epoch = timeClient.getEpochTime();    // Get Unix epoch time from the NTP server
  
  unsigned long unix_epoch = rtc.getEpoch();
  second_ = second(unix_epoch);
  if (last_second != second_) {
    isSecChange = true;
    minute_ = minute(unix_epoch);
    //hour_   = hour(unix_epoch);
    day_    = day(unix_epoch);
    month_  = month(unix_epoch);
    year_   = year(unix_epoch);
    
    hour_ = rtc.getHour(false);

    if(hour_ == 00)
        {
          hour_ = 12;
        }
    
    if (minute_ != last_min){
     isMinuteChange = true;
     last_min=minute_;
    }

    if (hour_ != last_hour){
     isHourChange = true;
     last_hour = hour_;
     updateRTC= true;
     if(hour_ == 00)
        {
          hour_ = 12;
        }
     _date = rtc.getDate(false);
     _ampm = rtc.getAmPm(false);

     if (lastAmPm != _ampm){
      lastAmPm = _ampm;
      isAmPmChange = true;
     }
    }
    if(_date != _last_date){
     _last_date = _date;
     isDateChange = true;
    }
    
   
    Time[4]  = minute_ % 10 + 48;
    Time[3]  = minute_ / 10 + 48;
    Time[1]  = hour_   % 10 + 48;
    Time[0]  = hour_   / 10 + 48;
 
 
 
    //Serial.println(Time);
    //Serial.println(Date);
    last_second = second_;
 
  }
   
}




// function to show date time 
void showDateTime(){
  
 if(isSecChange == true){
  isSecChange = false;
  tft.setTextColor(ST7796S_WHITE, ST7796S_BLACK);  
  tft.setTextSize(1); 
  tft.setFont(&FreeSansBold90pt7b);
  
  
  if(isHourChange == true){
    tft.setTextColor(ST7796S_BLACK);  
    tft.fillRect(10, 60, 200, 155, ST7796S_WHITE);
    isHourChange = false; 
    tft.setCursor(10, 200);
    tft.print(Time[0]);tft.print(Time[1]);
    //tft.setTextColor(ST7796S_RED); 
    //tft.print(":");
    }
  
  if(isMinuteChange == true){
   tft.setTextColor(ST7796S_BLACK); 
   tft.fillRect(270, 60, 200, 155, ST7796S_WHITE);
   isMinuteChange =false;
   tft.setCursor(270, 200);
   tft.print(Time[3]);tft.print(Time[4]);
  }

  if(isAmPmChange == true){
    isAmPmChange=false;
    tft.setFont(&FreeSansBold24pt7b); 
    tft.setTextColor(ST7796S_WHITE);
    tft.setCursor(380, 40);
    tft.fillRect(360, 0, 120, 50, ST7796S_BLACK);
    tft.print(_ampm);
  }  
  
  
  if(isDateChange == true){
    isDateChange = false; 
    tft.setFont(&FreeSansBold24pt7b); 
    tft.setTextColor(ST7796S_WHITE); 
    tft.fillRect(0, 250, 480, 120, ST7796S_BLACK);
    tft.setCursor(50, 300);
    tft.print(_date); 
   }  
 }
}
