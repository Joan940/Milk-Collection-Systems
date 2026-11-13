#ifndef RFID_H
#define RFID_H

// #include <SPI.h>
#include <MFRC522.h>

#define SDA_PIN 17
#define RST_PIN 14

class rfidModule {
  public:
    MFRC522 rfid; 
    byte lastUID[7];

    rfidModule() : rfid(SDA_PIN, RST_PIN) {
      for (int i = 0; i < 7; i++) lastUID[i] = 0;
    }

    void begin() {
      SPI.begin(18, 19, 23);
      rfid.PCD_Init();
      
      byte ver = rfid.PCD_ReadRegister(MFRC522::VersionReg);
      
      if (ver == 0x00 || ver == 0xFF) {
        Serial.println(F("2. GAGAL! Modul RC522 tidak terdeteksi atau salah jalur kabel."));
        while(1);
      }

      Serial.print(F("2. RFID BERHASIL DIKENALI!"));
      rfid.PCD_DumpVersionToSerial();
    }

    String getUID() {
      String uidString = "";
      
      if (!rfid.PICC_IsNewCardPresent()) return "";
      if (!rfid.PICC_ReadCardSerial()) return "";

      bool isDifferent = false;
      for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] != lastUID[i]){
          isDifferent = true;
          break;
        }
      }

      // if (isDifferent) {
      Serial.println(F("\n--- Kartu Terdeteksi ---"));
      
      for (byte i = 0; i < rfid.uid.size; i++) {
        lastUID[i] = rfid.uid.uidByte[i];
        if (rfid.uid.uidByte[i] < 0x10) uidString += "0";
        uidString += String(rfid.uid.uidByte[i], HEX);
        if (i < rfid.uid.size - 1) uidString += " ";
      }
      
      uidString.toUpperCase();
      Serial.println("UID : " + uidString);
      // }

      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();

      return uidString;
    }
};

#endif RFID_H