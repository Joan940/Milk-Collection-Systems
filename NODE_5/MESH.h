#ifndef MESH_H
#define MESH_H

#include <SPI.h>
#include <RHMesh.h>
#include <RH_RF95.h>

#include "MICROSD.h"
#include "LCD.h"

extern lcdModule myLcd;
extern microSDModule mySD;

#define LORA_CS    25
#define LORA_RST   26
#define LORA_DIO0  5

#define LORA_FREQ  433.0 
#define GATEWAY_ID 1

class meshComm {
  private:
    RH_RF95* rf95;
    RHMesh* manager;
    uint8_t myNodeId;
    
    void(*externalCallback)(uint8_t, String&);

  public:
    int retryCount = 0;
    String lastMessage = "";

    meshComm(uint8_t nodeId) {
      myNodeId = nodeId;
      rf95 = new RH_RF95(LORA_CS, LORA_DIO0);
      manager = new RHMesh(*rf95, myNodeId);
    }

    void begin(bool gatewayMode) {
      pinMode(LORA_RST, OUTPUT);
      digitalWrite(LORA_RST, HIGH);
      delay(10);
      digitalWrite(LORA_RST, LOW);
      delay(10);
      digitalWrite(LORA_RST, HIGH);
      delay(10);

      SPI.begin(18, 19, 23);

      if (!manager->init()) {
        Serial.println("1. [LORA_ERR] Inisialisasi LoRa Mesh Gagal!");
        while (1);
      }

      rf95->setFrequency(LORA_FREQ);
      rf95->setTxPower(2, false);

      // manager->addRouteTo(GATEWAY_ID, 4);

      manager->setTimeout(2000);
      manager->setRetries(0);

      Serial.print("1. [LORA_INIT] LoRa Mesh Siap. Node ID: ");
      Serial.println(myNodeId);
    }

    void update() {
      uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
      uint8_t from;

      if (manager->recvfromAck(buf, &len, &from)) {
        buf[len] = '\0';
        String msg = String((char*)buf);
        
        this->handleInternalMessage(from, msg);
      }
    }

    bool sendData(uint8_t targetNode, const String& data) {
      lastMessage = data;
      Serial.print("[LORA_TX] Mengirim data ke Node ");
      Serial.println(targetNode);
      
      return sendReply(targetNode, data); 
    }

    // bool sendData(const String& data) {
    //   lastMessage = data;
    //   Serial.println("[LORA_TX] Memulai pengiriman data (Sistem Estafet)...");
      
    //   uint8_t nodePerantara[] = {1, 4, 3, 2}; 
    //   int jumlahNode = sizeof(nodePerantara) / sizeof(nodePerantara[0]);

    //   for (int i = 0; i < jumlahNode; i++) {
    //     uint8_t targetNode = nodePerantara[i];
        
    //     Serial.print("[LORA_TX] Mencoba titip ke Node ");
    //     Serial.println(targetNode);

    //     bool sukses = sendReply(targetNode, data);

    //     if (sukses) {
    //       Serial.print("[LORA_TX] SUKSES! Data aman dititipkan di Node ");
    //       Serial.println(targetNode);
    //       return true; 
    //     } else {
    //       Serial.print("[LORA_ERR] Node ");
    //       Serial.print(targetNode);
    //       Serial.println(" gagal dihubungi. Mencari rute cadangan...");
    //     }
    //   }

    //   Serial.println("[LORA_ERR] FATAL: Semua Node Perantara (4, 3, 2) MATI!");
    //   return false;
    // }

    bool sendReply(uint8_t dest, const String& msg) {
      uint8_t error = manager->sendtoWait((uint8_t*)msg.c_str(), msg.length(), dest);
      
      if (error == RH_ROUTER_ERROR_NONE) {
        Serial.println("[LORA_TX] Sukses! Data tiba di Node " + String(dest));
        return true;
      } else {
        Serial.println("[LORA_ERR] Gagal mengirim. Kode Error: " + String(error));
        return false;
      }
    }

    void handleInternalMessage(uint8_t from, String &msg) {
      if (externalCallback) {
        externalCallback(from, msg);
      }
    }

    void setCallback(void (*f)(uint8_t, String&)) {
      externalCallback = f;
    }

    uint8_t getMyId() {
      return myNodeId;
    }
};

#endif MESH_H