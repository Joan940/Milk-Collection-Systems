// Library
#include <SPI.h>
#include <WiFi.h>
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

// Konfigurasi WiFi Komputer
const char* ssid = "MACBOOK";
const char* password = "12345678";
const char* ipKomputer = "192.168.137.1"; // WiFi
const int portKomputer = 5005;

// Konfigurasi WDT
#define WDT_TIMEOUT 30

// Inisialisasi Custom Library
aes myAES;
lcdModule myLcd;
rtcModule myRtc;
rfidModule myRfid;
meshComm myMesh(1);
microSDModule mySD;
lidarModule myLidar;
databaseModule myDb;

// VARIABLE
#define btn1 27
#define btn2 16
#define pinRfidCS 17
#define pinLoraCS 25
#define pinLcdCS 15
#define pinLoraRST 26
#define pinSdCS 13
#define pinRfidRST 14

bool choice = false;
unsigned long waktuSyncTerakhir = 0;
volatile bool adaPesanBaru = false;
volatile uint8_t pengirim = 0;
char bufferPesan[256];

// TAMBAHAN VARIABEL UNTUK MENCEGAH LORA ERROR & REBOOT
bool layarUtamaTampil = false;
unsigned long waktuCekRFID = 0;

const float phi = 3.14159265;
const float r = 12.4;

void dumpCS(const char* when) {
  Serial.print("[N1][CS] "); Serial.print(when);
  Serial.print(" LORA="); Serial.print(digitalRead(pinLoraCS));
  Serial.print(" LCD=");  Serial.print(digitalRead(pinLcdCS));
  Serial.print(" RFID="); Serial.print(digitalRead(pinRfidCS));
  Serial.print(" SD=");   Serial.print(digitalRead(pinSdCS));
  Serial.println();
}

void saatDataMasuk(uint8_t from, String& msg) {
  digitalWrite(pinLcdCS, HIGH);
  digitalWrite(pinRfidCS, HIGH);
  delay(10);

  String updateData = msg + " -> Node 1";
  // mySD.logToFile(msg);
  mySD.logToFile(updateData);

  digitalWrite(pinSdCS, HIGH);
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0)); 
  SPI.endTransaction();

  Serial.print(">> [LORA RX] Pesan masuk dari Node ");
  Serial.print(from);
  Serial.println(" -> Langsung diamankan ke SD Card.");
}

  // strncpy(bufferPesan, msg.c_str(), sizeof(bufferPesan) - 1);
  // bufferPesan[sizeof(bufferPesan) - 1] = '\0';
  
  // pengirim = from;
  // adaPesanBaru = true;

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

  // LCD
  myLcd.begin();
  myLcd.centerText("Systems Launch", 1);
  myLcd.show();
  delay(1000);

  // KONEKSI WIFI
  myLcd.clear();
  myLcd.centerText("Koneksi WiFi..", 1);
  myLcd.show();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");
  delay(1000);

  // RFID
  myLcd.clear();
  myRfid.begin();
  myLcd.centerText("2. RFID", 1);
  myLcd.show();
  delay(1000);

  // LORA MESH
  myLcd.clear();
  myMesh.begin(true);
  myMesh.setCallback(saatDataMasuk);
  myLcd.centerText("1. Node 1", 1);
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
  
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);
}

void loop() {
  esp_task_wdt_reset();

  digitalWrite(pinRfidCS, HIGH);
  digitalWrite(pinLcdCS, HIGH);
  digitalWrite(pinSdCS, HIGH);
  
  myMesh.update();

  if(millis() - waktuSyncTerakhir > 2000) {
    bool adaData = mySD.hasUnsentData();
    String dataTertunda = "";
    if(adaData) {
      dataTertunda = mySD.readNextLine();
    }

    if(adaData && dataTertunda != "") {
      digitalWrite(pinLoraCS, HIGH);
      digitalWrite(pinRfidCS, HIGH);
      myLcd.clear();
      myLcd.centerText("Sync ke PC...", 1);
      myLcd.show();
      delay(1000);

      bool statusKirim = kirimKeWiFi(dataTertunda);

      myRfid.rfid.PCD_Init();
      myLcd.begin();

      if (statusKirim == true) {
        mySD.removeLine();
        myLcd.clear();
        myLcd.centerText("Sync Berhasil!", 1);
        myLcd.show();
        delay(2000);
      } else {
        myLcd.clear();
        myLcd.centerText("WiFi Terputus!", 1);
        myLcd.show();
        delay(2000);
      }
      layarUtamaTampil = false;
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

      // KONFIRMASI UID UNTUK LANJUT KE PENGUKURAN
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
        // dumpCS("Baris 298");
        myMesh.update();

        if(digitalRead(btn1) == LOW) {
          while(digitalRead(btn1) == LOW) { esp_task_wdt_reset(); delay(50); }
          
          while(true) {
            esp_task_wdt_reset();

            // LIDAR
            long totalJarak = 0;
            int sampleValid = 0;
            for(int i = 0; i < 30; i++) {
              esp_task_wdt_reset();
              myMesh.update();

              int jarak = myLidar.jarakMentah();
              if(jarak != -1) {
                totalJarak += jarak;
                sampleValid++;

                myLcd.clear();
                String progress = "";
                for(int j = 0; j < (i % 5) + 1; j++) progress += ".";

                myLcd.centerText(progress, 2);
                myLcd.show();
              }
              delay(50);
            }

            if(sampleValid > 0) {
              hasilSMA = ((float)totalJarak / 30) / 10;
            } else {
              hasilSMA = 0;
            }

            int tinggiSusu = 50.0 - hasilSMA;
            if(tinggiSusu < 0) {
              tinggiSusu = 0;
            }

            volumeMentah = ((phi * pow(r, 2)) * tinggiSusu) / 1000;
            if(volumeMentah < 1.459) {
              volumeSusu = 0;
            } else {
              volumeSusu = (-0.0121 * pow(volumeMentah, 2)) + (1.3773 * volumeMentah) - 1.9829;
            }
            if (volumeSusu < 0 || volumeMentah == 0) {
              volumeSusu = 0;
            }

            // volumeSusu = (-0.0129 * pow(volumeMentah, 2)) + (1.3323 * volumeMentah) - 3.0489;

            myLcd.clear();
            String hasil = String(volumeSusu);
            myLcd.centerText(hasil, 2);
            myLcd.bottomLeft("ULANG", 1);
            myLcd.bottomRight("LANJUT", 1);
            myLcd.show();

            bool lock = true;
            while(true) {
              esp_task_wdt_reset();
              // dumpCS("Baris 357");
              myMesh.update();

              if(digitalRead(btn1) == LOW) {
                while(digitalRead(btn1) == LOW) { esp_task_wdt_reset(); delay(50); }
                break;
              }
              if(digitalRead(btn2) == LOW) {
                lock = false;
                myLcd.clear();
                myLcd.show();
                delay(1000);
                while(digitalRead(btn2) == LOW) { esp_task_wdt_reset(); delay(50); }
                break;
              }
              delay(100);
            }

            if(lock == true) {
              break;
            } else {
              continue;
            }

            delay(50);
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
        delay(100); 
      }

      // KONFIRMASI HASIL PENGUKURAN DAN LANJUT PENGIRIMAN
      int lastPressedBtn = 0;

      myLcd.clear();
      myLcd.validation(lastPressedBtn);
      myLcd.show();

      while(true) {
        esp_task_wdt_reset();
        // dumpCS("Baris 404");
        myMesh.update();

        if(digitalRead(btn1) == LOW) {
          if(lastPressedBtn == 1) {
            String finalData = "1-" + currentUID + "-" + String(volumeSusu) + "-" + myRtc.getTime();
            Serial.println(finalData);
            
            // WAKTU KONVERSI AES-256
            Serial.println();
            unsigned long waktuMulaiAES = micros();
            String dataEnkripsi = myAES.encrypt(finalData);
            unsigned long waktuSelesaiAES = micros();

            // HITUNG SELISIH WAKTU
            float waktuKomputasi = (waktuSelesaiAES - waktuMulaiAES) / 1000.0;
            Serial.print("Data Mentah         : ");
            Serial.println(finalData);
            Serial.print("Hasil Enkripsi      : ");
            Serial.println(dataEnkripsi);
            Serial.print("Waktu Komputasi AES : ");
            Serial.print(waktuKomputasi);
            Serial.println(" ms");
            Serial.println();

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

            bool statusKirim = kirimKeWiFi(dataEnkripsi);
            // myRfid.rfid.PCD_Init();

            if (statusKirim == true) {
              myLcd.clear();
              myLcd.pesanTerkirim();
              myLcd.show();
              delay(3000); 
            } else {
              // MATIKAN LORA & RFID SEBELUM SD CARD MENULIS
              digitalWrite(pinLoraCS, HIGH);
              digitalWrite(pinRfidCS, HIGH);
              delay(50);
              
              mySD.logToFile(dataEnkripsi);
              digitalWrite(pinSdCS, HIGH);
              SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0)); 
              SPI.endTransaction();
              
              waktuMulai = millis();
              while(millis() - waktuMulai < 3000) {
                esp_task_wdt_reset();
                myLcd.clear();
                myLcd.drawSave();
                myLcd.show();
                delay(50);
              }
            }

            myLcd.clear();
            myLcd.show();
            delay(1000);
            layarUtamaTampil = false;
            break;
          }

          lastPressedBtn = 1;
          myLcd.clear();
          myLcd.validation(lastPressedBtn);
          myLcd.show();
          while(digitalRead(btn1) == LOW) { esp_task_wdt_reset(); delay(50); }
        }

        if(digitalRead(btn2) == LOW) {
          if(lastPressedBtn == 2) {
            myLcd.clear();
            myLcd.centerText("Dibatalkan", 1);
            myLcd.show();
            delay(2000);
            layarUtamaTampil = false;
            break;
          }

          lastPressedBtn = 2;
          myLcd.clear();
          myLcd.validation(lastPressedBtn);
          myLcd.show();
          while(digitalRead(btn2) == LOW) { esp_task_wdt_reset(); delay(50); }
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

bool kirimKeWiFi(String payload) {
  if(WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    client.setTimeout(3000);

    esp_task_wdt_reset();

    if(client.connect(ipKomputer, portKomputer)) {
      client.print(payload);
      client.stop();
      Serial.println("[WIFI_TX] Sukses kirim ke PC via TCP: " + payload);
      return true;
    } else {
      esp_task_wdt_reset();
      Serial.println("[WIFI_ERR] Koneksi TCP Ditolak! (PC mati atau Port 5005 tertutup)");
      return false;
    }
  } else {
    Serial.println("[WIFI_ERR] WiFi Terputus! Gagal mengirim data.");
    return false;
  }
}