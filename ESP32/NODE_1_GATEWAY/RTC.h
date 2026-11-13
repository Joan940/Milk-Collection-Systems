#ifndef RTC_H
#define RTC_H

#include <Wire.h>
#include <RTClib.h>

extern lcdModule myLcd;

class rtcModule {
  public:
    RTC_DS3231 rtc;

    void begin() {
      if (!rtc.begin()) {
        myLcd.centerText("4. RTC Error!");
        while(1);
      }
  
      if (rtc.lostPower()) {
        Serial.println("4. Daya RTC lemah, sinkronisasi waktu kembali!");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      }

      // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

      Serial.println("4. RTC DIKENALI!");
    }

    String getTime() {
      DateTime now = rtc.now();

      String tanggal = (now.day() < 10 ? "0" : "") + String(now.day());
      String bulan   = (now.month() < 10 ? "0" : "") + String(now.month());
      String tahun   = String(now.year());
      
      String jam     = (now.hour() < 10 ? "0" : "") + String(now.hour());
      String menit   = (now.minute() < 10 ? "0" : "") + String(now.minute());

      String waktu = tanggal + "/" + bulan + "/" + tahun + " " + jam + ":" + menit;

      return waktu;
    }
};

#endif RTC_H