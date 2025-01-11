/*
ESP32C3 + 16*2のLCDで時刻取得して表示するやつ
*/


#include <WiFi.h>
#include <Wire.h>
#include "time.h"
#include "esp_sntp.h"
#include "LiquidCrystal_I2C.h"

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

LiquidCrystal_I2C lcd(0x3F, 16, 2);

char jpconv[64];
char* jp(char *text) {
    char *cur = &jpconv[0], *chk = text;
    while (*chk != 0) {
        if ((*chk & 0xff) == 0xef && (*(chk+1) & 0xbc) == 0xbc) {
            *cur = (*++chk & 0x02) << 5;
            *cur++ = *++chk + *cur;
        } else
            *cur++ = *chk;
        chk++;
    }
    *cur = 0;
    return jpconv;
}

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
  lcd.init();
  lcd.backlight();

  struct tm timeinfo;

  if (getLocalTime(&timeinfo)) {

    year = timeinfo.tm_year + 1900;
    month = timeinfo.tm_mon + 1;
    day = timeinfo.tm_mday;
    hour = timeinfo.tm_hour;
    minute = timeinfo.tm_min;

    char yymmdd[64];
    sprintf(yymmdd, "%04d/%02d/%02d", year, month, day);

    char hhmm[128];
    sprintf(hhmm, "%02d:%02d", hour, minute);
    
  
    char nanao[256];
    String nanaohead = jp("ﾕﾘﾁｬﾝｶﾞ");
    sprintf(nanao, "%s%02d:%02d%s", nanaohead, hour, minute, jp("ｦ"));
    String nanao2 = jp("ｵｼﾗｾｼﾏｽﾖ ｲｸｯ");


    lcd.setCursor(0, 0);
    lcd.print(nanao);
    lcd.setCursor(0, 1);
    lcd.print(nanao2);

  }
  //printLocalTime();
}

