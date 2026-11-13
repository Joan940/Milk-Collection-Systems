#ifndef MICROSD_H
#define MICROSD_H

#include <SD.h>
#include <SPI.h>

#define CS_PIN 13
#define HSPI_SCK  33
#define HSPI_MISO 35
#define HSPI_MOSI 32

class microSDModule {
  private:
    SPIClass *hspi = nullptr;

  public:
    const char* path = "/Pengiriman Gagal/log_tps_4.txt";
    const char* tempPath = "/Pengiriman Gagal/temp.txt";

    void begin() {
      hspi = new SPIClass(HSPI);
      hspi->begin(HSPI_SCK, HSPI_MISO, HSPI_MOSI, CS_PIN);

      if(!SD.begin(CS_PIN, *hspi)) {
        Serial.println("3. SD card ERROR!");
        while(1);
      }
      Serial.println("3. MICRO SD BERHASIL DIKENALI!");
    }

    void createNewFile() {
      if(!SD.exists("/Pengiriman Gagal")) {
        SD.mkdir("/Pengiriman Gagal");
      }

      if (!SD.exists(path)) {
        File myFile = SD.open(path, FILE_WRITE);
        if (myFile) {
          myFile.close();
          Serial.println("[SD] File antrean baru berhasil dibuat (Murni Kosong).");
        } else {
          Serial.println("[SD_ERR] Gagal membuat file baru!");
        }
      }
    }

    void logToFile(String message) {
      File file = SD.open(path, FILE_APPEND);
      if(!file) {
        Serial.println("Gagal membuka file!");
        return;
      }

      if(file) {
        file.println(message);
        file.flush();
        file.close();
      }
    }

    bool hasUnsentData() {
      File file = SD.open(path, FILE_READ);
      if (!file) {
        return false;
      }
      
      bool hasData = (file.size() > 0);
      file.close();
      
      return hasData;
    }

    String readNextLine() {
      File file = SD.open(path, FILE_READ);
      if (!file) return "";

      String line = file.readStringUntil('\n');
      line.trim();
      
      file.close();
      return line;
    }

    void removeLine() {
      File originalFile = SD.open(path, FILE_READ);
      if (!originalFile) return;

      File tempFile = SD.open("/Pengiriman Gagal/temp.txt", FILE_WRITE);
      if (!tempFile) {
        originalFile.close();
        return;
      }

      bool isFirstLineSkipped = false;

      while (originalFile.available()) {
        String line = originalFile.readStringUntil('\n');
        
        line.trim();

        if (!isFirstLineSkipped) {
          isFirstLineSkipped = true;
          continue; 
        }
        
        if (line.length() > 0) {
          tempFile.print(line + "\n");
          tempFile.flush();
        }
      }

      originalFile.close();
      tempFile.close();

      if(SD.remove(path)) {
        if(SD.rename(tempPath, path)) {
          Serial.println("[SD] Antrean pertama berhasil dihapus, sisa data maju ke depan.");
        } else {
          Serial.println("[SD_ERR] Gagal me-rename file temp!");
        }
      } else {
        Serial.println("[SD_ERR] Gagal menghapus file log lama!");
      }
    }
};

#endif MICROSD_H