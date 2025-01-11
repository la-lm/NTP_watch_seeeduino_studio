/*
秋月で売ってる8*2のLCD版
*/


#include <WiFi.h>
#include <Wire.h>
#include "time.h"
#include "esp_sntp.h"
#include "I2CLiquidCrystal.h"

const char* ssid       = "YOUR_SSID";
const char* password   = "YOUR_PASSWORD";

const char* ntpServer1 = "ntp.nict.jp";
const char* ntpServer2 = "time.google.com";
const char* ntpServer3 = "ntp.jst.mfeed.ad.jp";
const long  gmtOffset_sec = 9 * 3600;
const int   daylightOffset_sec = 0;

int year = 0;
int month = 0;
int day = 0;
int hour = 0;
int minute = 0;

I2CLiquidCrystal lcd(20, false);

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval *t)
{
  Serial.println("Got time adjustment from NTP!");
}

void setup()
{
  Serial.begin(115200);
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  sntp_set_time_sync_notification_cb( timeavailable );
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2, ntpServer3);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop()
{
  delay(10000);
  lcd.begin(8, 2);

  struct tm timeinfo;

  if (getLocalTime(&timeinfo)) {

    year = timeinfo.tm_year;
    month = timeinfo.tm_mon + 1;
    day = timeinfo.tm_mday;
    hour = timeinfo.tm_hour;
    minute = timeinfo.tm_min;

    char mmdd[64];
    sprintf(mmdd, "%02d/%02d", month, day);

    char hhmm[64];
    sprintf(hhmm, "%02d:%02d", hour, minute);

    lcd.setCursor(0, 0);
    lcd.print(mmdd);
    lcd.setCursor(0, 1);
    lcd.print(hhmm);

  }
  //printLocalTime();
}