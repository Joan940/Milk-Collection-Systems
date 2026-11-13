// Library
#include <SPI.h>
#include <esp_task_wdt.h>

// Custom Library
#include "LCD.h"
#include "AES.h"
#include "RTC.h"
#include "MESH.h"
#include "RFID.h"
#include "LIDAR.h"
#include "MICROSD.h"
#include "DATABASE.h"

// Konfigurasi WDT
#define WDT_TIMEOUT

// Inisialisasi Custom Library
aes myAES;
lcdModule myLcd;
rtcModule myRtc;
rfidModule myRfid;
meshComm myMesh(4);
microSDModule mySD;
lidarModule myLidar;
databaseModule myDb;

// VARIABLE
#define btn1 34
#define btn2 27
#define pinRfidCS 17
#define pinLoraCS 25
#define pinLcdCS 15
#define pinLoraRST 26
#define pinSdCS 13
#define pinPowerRFID 14

bool choice = false;
unsigned long waktuSyncTerakhir = 0;
const float phi = 3.14159265;
const float r = 12.1;

// TAMBAHAN VARIABEL UNTUK STABILITAS SPI
bool layarUtamaTampil = false;
unsigned long waktuCekRFID = 0;

void saatDataMasuk(uint8_t from, String& msg) {
  digitalWrite(pinLoraCS, HIGH);
  digitalWrite(pinLcdCS, HIGH);
  digitalWrite(pinRfidCS, HIGH);
  delay(10);

  String updateData = msg + " -> Node 4";
  mySD.logToFile(updateData);

  digitalWrite(pinSdCS, HIGH);
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0)); 
  SPI.endTransaction();

  Serial.print(">> [LORA RX] Pesan masuk dari Node ");
  Serial.print(from);
  Serial.println(" -> Langsung diamankan ke SD Card.");
}

void setup() {
  delay(3000);

  pinMode(pinRfidCS, OUTPUT);
  pinMode(pinLoraCS, OUTPUT);
  pinMode(pinLcdCS, OUTPUT);
  pinMode(pinLoraRST, OUTPUT);
  pinMode(pinSdCS, OUTPUT);

  digitalWrite(pinRfidCS, HIGH);
  digitalWrite(pinLoraCS, HIGH);
  digitalWrite(pinLcdCS, HIGH);
  digitalWrite(pinSdCS, HIGH);
  digitalWrite(pinLoraRST, HIGH);
  delay(100);
  
  Serial.begin(115200);
  Serial.println();

  // PUSH BUTTON
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);

  SPI.begin(18, 19, 23);

  // LCD
  myLcd.begin();
  myLcd.centerText("Systems Launch", 1);
  myLcd.show();
  delay(1000);

  // LORA MESH
  myLcd.clear();
  myMesh.begin(false);
  myMesh.setCallback(saatDataMasuk);
  myLcd.centerText("1. Node 5", 1);
  myLcd.show();
  delay(1000);

  // RFID
  myLcd.clear();
  myRfid.begin();
  myLcd.centerText("2. RFID", 1);
  myLcd.show();
  delay(1000);
  
  // MICRO SD
  myLcd.clear();
  mySD.begin();
  myLcd.centerText("3. Micro SD", 1);
  mySD.createNewFile();
  myLcd.show();
  delay(1000);

  // RTC
  myLcd.clear();
  myRtc.begin();
  myLcd.centerText("4. RTC", 1);
  myLcd.show();
  delay(1000);

  // LIDAR
  myLcd.clear();
  myLidar.begin();
  myLcd.centerText("5. LiDAR", 1);
  myLcd.show();
  delay(1000);
  
  // // ---> TAMBAHAN 3: Inisialisasi WDT
  // esp_task_wdt_init(WDT_TIMEOUT, true); 
  // esp_task_wdt_add(NULL); 
}

void loop() {
  esp_task_wdt_reset();

  digitalWrite(pinRfidCS, HIGH);
  digitalWrite(pinLcdCS, HIGH);
  digitalWrite(pinSdCS, HIGH);
  // Serial.println("Sampe sini");
  myMesh.update();

  if(millis() - waktuSyncTerakhir > 5000) {
    bool adaData = mySD.hasUnsentData();
    String dataTertunda = "";
    if(adaData) {
      dataTertunda = mySD.readNextLine();
    }

    if(adaData && dataTertunda != "") {
      digitalWrite(pinLoraCS, HIGH);
      digitalWrite(pinRfidCS, HIGH);
      myLcd.clear();
      myLcd.centerText("Sinkronisasi...", 1);
      myLcd.show();

      digitalWrite(pinRfidCS, HIGH);
      digitalWrite(pinLcdCS, HIGH);
      digitalWrite(pinSdCS, HIGH);
      delay(50);

      SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0)); 
      SPI.endTransaction();

      bool statusKirim = myMesh.sendData(dataTertunda);

      if (statusKirim == true) {
        mySD.removeLine();
        myLcd.clear();
        myLcd.centerText("Sync Berhasil!", 1);
        myLcd.show();
        delay(2000);
      } 

      // digitalWrite(pinSdCS, HIGH);
      // digitalWrite(pinLoraCS, HIGH);
      // delay(50);
      // myRfid.rfid.PCD_Init();
      // myLcd.begin();
      
      layarUtamaTampil = false; // BUKA KUNCI LCD
    }
    waktuSyncTerakhir = millis(); 
  }

  if (layarUtamaTampil == false) {
    digitalWrite(pinLoraCS, HIGH);
    digitalWrite(pinRfidCS, HIGH);
    digitalWrite(pinSdCS, HIGH);
    
    myLcd.clear();
    myLcd.drawScan();
    myLcd.show();
    
    digitalWrite(pinLcdCS, HIGH);
    layarUtamaTampil = true; 
  }

  String currentUID = "";
  if(millis() - waktuCekRFID > 300) {
    digitalWrite(pinLoraCS, HIGH);
    digitalWrite(pinLcdCS, HIGH);
    digitalWrite(pinSdCS, HIGH);
    
    currentUID = myRfid.getUID();
    
    digitalWrite(pinRfidCS, HIGH);
    waktuCekRFID = millis();
  }

  if(currentUID != "") {
    myRfid.rfid.PICC_HaltA();
    myRfid.rfid.PCD_StopCrypto1();

    layarUtamaTampil = false;
    
    String username = myDb.getName(currentUID);
    if(username != "Tidak ada") {
      myLcd.clear();
      myLcd.centerText(username, 1);
      myLcd.show();

      delay(1700);

      myLcd.clear();
      myLcd.centerText("MULAI UKUR?", 1);
      myLcd.bottomLeft("TIDAK", 1);
      myLcd.bottomRight("YA", 1);
      myLcd.show();

      float hasilSMA = 0;
      float volumeSusu = 0;
      float volumeMentah = 0;
      while(true) {
        esp_task_wdt_reset();
        myMesh.update();

        if(digitalRead(btn1) == LOW) {
          while(digitalRead(btn1) == LOW) { esp_task_wdt_reset(); delay(100); }
          
          while(true) {
            esp_task_wdt_reset();

            // LIDAR
            long totalJarak = 0;
            int sampleValid = 0;
            for(int i = 0; i < 30; i++) {
              esp_task_wdt_reset();
              myMesh.update();

              long jarak = myLidar.jarakMentah();
              if(jarak != -1) {
                totalJarak += jarak;
                sampleValid++;

                myLcd.clear();
                String progress = "";
                for(int j = 0; j < (i % 5) + 1; j++) progress += ".";

                myLcd.centerText(progress, 2);
                myLcd.show();
              }
              delay(100);
            }

            if(sampleValid > 0) {
              hasilSMA = ((float)totalJarak / 30) / 10;
            // int jarakArray[30]; 
            // int sampleValid = 0;
            
            // for(int i = 0; i < 30; i++) {
            //   esp_task_wdt_reset(); 
            //   myMesh.update();

            //   long jarak = myLidar.jarakMentah();
            //   if(jarak != -1) {
            //     jarakArray[sampleValid] = jarak;
            //     sampleValid++;

            //     myLcd.clear();
            //     String progress = "";
            //     for(int j = 0; j < (i % 5) + 1; j++) progress += ".";
            //     myLcd.centerText(progress, 2);
            //     myLcd.show();
            //   }
            //   delay(100);
            // }

            // if(sampleValid > 0) {
            //   for (int i = 0; i < sampleValid - 1; i++) {
            //     for (int j = 0; j < sampleValid - i - 1; j++) {
            //       if (jarakArray[j] > jarakArray[j + 1]) {
            //         int temp = jarakArray[j];
            //         jarakArray[j] = jarakArray[j + 1];
            //         jarakArray[j + 1] = temp;
            //       }
            //     }
            //   }

            //   float nilaiTengah;
            //   if (sampleValid % 2 == 0) {
            //     nilaiTengah = (jarakArray[(sampleValid / 2) - 1] + jarakArray[sampleValid / 2]) / 2.0;
            //   } else {
            //     nilaiTengah = jarakArray[sampleValid / 2];
            //   }

            //   hasilSMA = nilaiTengah / 10.0;
            } else {
              hasilSMA = 0;
            }

            float tinggiSusu = 52.70 - hasilSMA;
            if (tinggiSusu < 0) {
              tinggiSusu = 0;
            }

            volumeMentah = ((phi * pow(r, 2)) * tinggiSusu) / 1000;
            volumeSusu = (-0.0026 * pow(volumeMentah, 2)) + (1.0302 * volumeMentah) - 0.354;
            if (volumeSusu < 0 || volumeMentah == 0) {
              volumeSusu = 0;
            }

            myLcd.clear();
            String hasil = String(volumeSusu);
            myLcd.centerText(hasil, 2);
            myLcd.bottomLeft("ULANG", 1);
            myLcd.bottomRight("LANJUT", 1);
            myLcd.show();

            bool lock = true;
            while(true) {
              esp_task_wdt_reset();
              myMesh.update();

              if(digitalRead(btn1) == LOW) {
                while(digitalRead(btn1) == LOW) { esp_task_wdt_reset(); delay(100); }
                break;
              }
              if(digitalRead(btn2) == LOW) {
                lock = false;

                myLcd.clear();
                myLcd.show();
                delay(1000);

                while(digitalRead(btn2) == LOW) { esp_task_wdt_reset(); delay(100); }
                break;
              }
              delay(10);
            }

            if(lock == true) {
              break;
            } else {
              continue;
            }
          }
          break;
        }

        if(digitalRead(btn2) == LOW) {
          myLcd.clear();
          myLcd.centerText("Dibatalkan", 1);
          myLcd.show();
          delay(2000);
          layarUtamaTampil = false;
          return;
        }
        delay(10);
      }

      int lastPressedBtn = 0;

      myLcd.clear();
      myLcd.validation(lastPressedBtn);
      myLcd.show();

      while(true) {
        esp_task_wdt_reset();
        myMesh.update(); 
        
        if(digitalRead(btn1) == LOW) {
          if(lastPressedBtn == 2) {
            String finalData = "4-" + currentUID + "-" + String(volumeSusu) + "-" + myRtc.getTime();
            String dataEnkripsi = myAES.encrypt(finalData);
            String dataTes = "Node 4";

            unsigned long waktuMulai = millis();
            while(millis() - waktuMulai < 3000) {
              esp_task_wdt_reset();
              myMesh.update();
              myLcd.clear();
              myLcd.drawKirim();
              myLcd.show();
              delay(100);
            }

            myLcd.clear();
            myLcd.show();
            yield();

            // MATIKAN SEMUA SEBELUM LORA BEKERJA
            digitalWrite(pinRfidCS, HIGH);
            digitalWrite(pinLcdCS, HIGH);
            digitalWrite(pinSdCS, HIGH);
            delay(50);
            
            esp_task_wdt_reset();
            bool statusKirim = myMesh.sendData(dataTes);
            // bool statusKirim = myMesh.sendData(1, dataEnkripsi);
            esp_task_wdt_reset();

            // FITUR AUTO-RETRY: Jika gagal (Error 2/5), coba lagi 1x
            // if (statusKirim == false) {
            //   Serial.println("[SYSTEM] Gagal kirim. Mencoba ulang (Retry) dalam 1 detik...");
            //   delay(1000);
            //   myMesh.update();
              
            //   digitalWrite(pinLoraCS, LOW);
            //   statusKirim = myMesh.sendData(dataEnkripsi);
            //   digitalWrite(pinLoraCS, HIGH);
            // }

            // myRfid.rfid.PCD_Init();
            // myLcd.begin();

            if (statusKirim == true) {
              myLcd.clear();
              myLcd.pesanTerkirim();
              myLcd.show();
              delay(2000); 
            } else {
              // MATIKAN LORA & RFID SEBELUM SD CARD MENULIS
              digitalWrite(pinLoraCS, HIGH);
              digitalWrite(pinRfidCS, HIGH);
              digitalWrite(pinLcdCS, HIGH);
              delay(50);
              
              mySD.logToFile(dataTes);

              digitalWrite(pinSdCS, HIGH);
              SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0)); 
              SPI.endTransaction();
              
              waktuMulai = millis();
              while(millis() - waktuMulai < 2000) {
                esp_task_wdt_reset();
                myLcd.clear();
                myLcd.drawSave();
                myLcd.show();
                delay(100);
              }
            }

            myLcd.clear();
            myLcd.show();
            delay(1000);
            layarUtamaTampil = false;
            break;
          }

          lastPressedBtn = 2;
          myLcd.clear();
          myLcd.validation(lastPressedBtn);
          myLcd.show();
          while(digitalRead(btn1) == LOW) { esp_task_wdt_reset(); delay(100); }
        }

        if(digitalRead(btn2) == LOW) {
          if(lastPressedBtn == 1) {
            myLcd.clear();
            myLcd.centerText("Dibatalkan", 1);
            myLcd.show();
            delay(2000);
            layarUtamaTampil = false;
            break;
          }

          lastPressedBtn = 1;
          myLcd.clear();
          myLcd.validation(lastPressedBtn);
          myLcd.show();
          while(digitalRead(btn2) == LOW) { esp_task_wdt_reset(); delay(500); }
        }

        delay(100);
      }
    } else {
      myLcd.clear();
      myLcd.centerText("AKSES DITOLAK!", 1);
      myLcd.show();
      delay(1700);
      layarUtamaTampil = false;
    }
  }

  delay(10);
}