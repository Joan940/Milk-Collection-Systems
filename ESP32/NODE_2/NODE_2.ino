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
#define WDT_TIMEOUT 30

// Inisialisasi Custom Library
aes myAES;
lcdModule myLcd;
rtcModule myRtc;
rfidModule myRfid;
meshComm myMesh(2);
microSDModule mySD;
lidarModule myLidar;
databaseModule myDb;

// Deklarasi Pin
#define btn1 16
#define btn2 27
#define pinRfidCS 17
#define pinLoraCS 25
#define pinLcdCS 15
#define pinLoraRST 26
#define pinSdCS 13
#define pinPowerRFID 14

// Variabel Global
bool choice = false;
const float r = 12.3;
const float phi = 3.14159265;
bool layarUtamaTampil = false;
unsigned long waktuCekRFID = 0;
unsigned long waktuSyncTerakhir = 0;

void dumpCS(const char* when) {
  Serial.print("[N2][CS] "); Serial.print(when);
  Serial.print(" LORA="); Serial.print(digitalRead(pinLoraCS));
  Serial.print(" LCD=");  Serial.print(digitalRead(pinLcdCS));
  Serial.print(" RFID="); Serial.print(digitalRead(pinRfidCS));
  Serial.print(" SD=");   Serial.print(digitalRead(pinSdCS));
  Serial.println();
}

volatile bool adaTitipanN3 = false;
String pesanTitipanN3 = "";

void saatDataMasuk(uint8_t from, String& msg) {
  digitalWrite(pinLoraCS, HIGH);
  digitalWrite(pinLcdCS, HIGH);
  digitalWrite(pinRfidCS, HIGH);
  delay(10);

  if (from == 3) {
    pesanTitipanN3 = msg + " -> Node 2";
    adaTitipanN3 = true;
    Serial.println(">> [LORA RX] Pesan dari Node 3 DITAMPUNG (Siap Relay).");
  } else {
    String updateData = msg + " -> Node 2";
    mySD.logToFile(updateData);
    Serial.println(">> [LORA RX] Disimpan ke SD Node 2.");
  }

  digitalWrite(pinSdCS, HIGH);
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0)); 
  SPI.endTransaction();
}

// void saatDataMasuk(uint8_t from, String& msg) {
//   digitalWrite(pinLoraCS, HIGH);
//   digitalWrite(pinLcdCS, HIGH);
//   digitalWrite(pinRfidCS, HIGH);
//   delay(10);

//   String updateData = msg + " -> Node 2";
//   mySD.logToFile(updateData);

//   digitalWrite(pinSdCS, HIGH);
//   SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0)); 
//   SPI.endTransaction();

//   Serial.print(">> [LORA RX] Pesan masuk dari Node ");
//   Serial.print(from);
//   Serial.println(" -> Langsung diamankan ke SD Card.");
// }

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
  myLcd.centerText("1. Node 2", 1);
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
}

void loop() {
  esp_task_wdt_reset();

  // static unsigned long tHb = 0;
  // if (millis() - tHb > 11000) {
  //   tHb = millis();
  //   dumpCS("Waktu heartbeat");
  //   myMesh.sendData("HB-2");
  // }

  // Jalur SPI Dikhususkan untuk LoRa
  digitalWrite(pinRfidCS, HIGH);
  digitalWrite(pinLcdCS, HIGH);
  digitalWrite(pinSdCS, HIGH);
  // dumpCS("Baris 154");
  myMesh.update();

  if(adaTitipanN3) {
    String dataForward = pesanTitipanN3;
    adaTitipanN3 = false;
    pesanTitipanN3 = "";

    myLcd.clear();
    myLcd.centerText("Merelay ke N1", 1);
    myLcd.show();
    delay(500);

    digitalWrite(pinRfidCS, HIGH);
    digitalWrite(pinLcdCS, HIGH);
    digitalWrite(pinSdCS, HIGH);
    delay(50);
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0)); 
    SPI.endTransaction();

    digitalWrite(pinLoraRST, LOW);
    delay(20);
    digitalWrite(pinLoraRST, HIGH);
    delay(50);
    myMesh.begin(false);

    bool suksesRelay = myMesh.sendData(dataForward);

    if(suksesRelay) {
      Serial.println(">> [RELAY] Sukses! Data tiba di N1.");
      myLcd.clear();
      myLcd.centerText("Relay OK!", 1);
      myLcd.show();
    } else {
      Serial.println(">> [RELAY ERR] Gagal. Selamatkan ke SD Backup.");
      mySD.logToFile(dataForward);
      myLcd.clear();
      myLcd.centerText("Gagal, Backup", 1);
      myLcd.show();
    }
    myLcd.clear();
    myLcd.show();
    delay(1500);
  }

  // Sinkronisasi Data di MicroSD
  static unsigned long interval = 2000;
  esp_task_wdt_reset();
  if(millis() - waktuSyncTerakhir > interval) {
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

      delay(500);

      digitalWrite(pinRfidCS, HIGH);
      digitalWrite(pinLcdCS, HIGH);
      digitalWrite(pinSdCS, HIGH);
      delay(50);

      SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0)); 
      SPI.endTransaction();

      digitalWrite(pinLoraRST, LOW);
      delay(20);
      digitalWrite(pinLoraRST, HIGH);
      delay(50);

      myMesh.begin(false);

      // dumpCS("Baris 179");
      bool statusKirim = myMesh.sendData(dataTertunda);

      if (statusKirim == true) {
        mySD.removeLine();
        myLcd.clear();
        myLcd.centerText("Sync Berhasil!", 1);
        myLcd.show();
        delay(2000);
      } 

      myMesh.setCallback(saatDataMasuk);

      // digitalWrite(pinSdCS, HIGH);
      // digitalWrite(pinLoraCS, HIGH);
      // delay(50);
      myRfid.rfid.PCD_Init();
      myLcd.begin();
      
      layarUtamaTampil = false;
    }
    waktuSyncTerakhir = millis(); 
    // interval = random(4000, 7000);
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

      // Konfirmasi KTP
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
        // dumpCS("Baris 253");
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
              // dumpCS("Baris 266");
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
            } else {
              hasilSMA = 0;
            }

            float tinggiSusu = 51.0 - hasilSMA;
            if (tinggiSusu < 0) {
              tinggiSusu = 0;
            }

            volumeMentah = ((phi * pow(r, 2)) * tinggiSusu) / 1000;
            volumeSusu = (-0.0037 * pow(volumeMentah, 2)) + (1.0615 * volumeMentah) + 0.0664;
            // volumeSusu = (-0.0063 * pow(volumeMentah, 2)) + (1.1516 * volumeMentah) - 1.1082;
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
              // dumpCS("Baris 352");
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

      // KONFIRMASI HASIL PENGUKURAN DAN LANJUT PENGIRIMAN
      int lastPressedBtn = 0;

      myLcd.clear();
      myLcd.validation(lastPressedBtn);
      myLcd.show();

      while(true) {
        esp_task_wdt_reset();
        // dumpCS("Baris 401");
        myMesh.update(); 
        
        if(digitalRead(btn1) == LOW) {
          if(lastPressedBtn == 2) {
            String finalData = "2-" + currentUID + "-" + String(volumeSusu) + "-" + myRtc.getTime();
            String dataEnkripsi = myAES.encrypt(finalData);
            String dataTes = "Node 2";

            unsigned long waktuMulai = millis();
            while(millis() - waktuMulai < 3000) {
              esp_task_wdt_reset();
              // dumpCS("Baris 412");
              myMesh.update();
              myLcd.clear();
              myLcd.drawKirim();
              myLcd.show();
              delay(100);
            }

            myLcd.clear();
            myLcd.show();
            yield();

            // MATIKAN SEMUA BUAT LORA
            digitalWrite(pinRfidCS, HIGH);
            digitalWrite(pinLcdCS, HIGH);
            digitalWrite(pinSdCS, HIGH);
            delay(100);
            
            esp_task_wdt_reset();
            // dumpCS("Baris 432");
            bool statusKirim = myMesh.sendData(dataTes);
            // bool statusKirim = myMesh.sendData(dataEnkripsi);
            esp_task_wdt_reset();

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
              // mySD.logToFile(dataEnkripsi);

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