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

#define NODE_TAG "N3"

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

    void begin(bool gatewayMode = true) {
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
      rf95->setTxPower(20, false);

      manager->addRouteTo(2, 2);

      manager->setTimeout(2000);
      manager->setRetries(0);

      Serial.print("1. [LORA_INIT] LoRa Mesh Siap. Node ID (Gateway): ");
      Serial.println(myNodeId);

      dumpRoutes("Pada saat inisialisasi LoRa");
    }

    void update() {
      uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
      uint8_t from;

      if (manager->recvfromAck(buf, &len, &from)) {
        buf[len] = '\0';
        String msg = String((char*)buf);

        Serial.print("["); Serial.print(NODE_TAG); Serial.print("][RX] t=");
        Serial.print(millis());
        Serial.print(" from="); Serial.print(from);
        Serial.print(" len="); Serial.print(len);
        Serial.print(" rssi="); Serial.print(rf95->lastRssi());
        Serial.print(" msg="); Serial.println(msg);

        handleInternalMessage(from, msg);
      }
    }

    bool sendData(const String& data) {
      lastMessage = data;
      Serial.println("[LORA_TX] Mempersiapkan data untuk Node 2");
      
      return sendReply(2, data); 
    }

    bool sendReply(uint8_t dest, const String& msg) {
      Serial.println();
      Serial.print("["); Serial.print(NODE_TAG); Serial.print("][TX] t=");
      Serial.print(millis());
      Serial.print(" dest="); Serial.print(dest);
      Serial.print(" len="); Serial.print(msg.length());
      Serial.print(" payload="); Serial.println(msg);

      digitalWrite(LORA_RST, LOW);
      delay(20);
      digitalWrite(LORA_RST, HIGH);
      delay(50);
      
      // Inisialisasi ulang tanpa perlu setting ulang gateway
      if (!manager->init()) {
         Serial.println("[N3][TX_ERR] Gagal re-init LoRa!");
         return false;
      }
      rf95->setFrequency(LORA_FREQ);
      rf95->setModemConfig(RH_RF95::Bw125Cr45Sf128); 
      rf95->setTxPower(20, false);
      manager->setTimeout(2000);
      manager->setRetries(3);

      manager->addRouteTo(2, 2);

      unsigned long t0 = millis();
      dumpRoutes("Sebelum sentoWait LoRa");
      uint8_t error = manager->sendtoWait((uint8_t*)msg.c_str(), msg.length(), dest);
      dumpRoutes("Setelah sentoWait LoRa");
      unsigned long dt = millis() - t0;

      Serial.print("["); Serial.print(NODE_TAG); Serial.print("][TX] selesai dt=");
      Serial.print(dt); Serial.print(" ms err=");
      Serial.print(error); Serial.print(" ");
      Serial.println(meshErrName(error));

      Serial.print("["); Serial.print(NODE_TAG); Serial.print("][RF] lastRssi=");
      Serial.print(rf95->lastRssi());
      Serial.print(" dBm  txPower_setting_lihat_begin");
      Serial.println();

      return error == RH_ROUTER_ERROR_NONE;
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

    const char* meshErrName(uint8_t e) {
      switch (e) {
        case RH_ROUTER_ERROR_NONE:            return "NONE(0) sukses";
        case RH_ROUTER_ERROR_INVALID_LENGTH:  return "INVALID_LEN(1)";
        case RH_ROUTER_ERROR_NO_ROUTE:        return "NO_ROUTE(2) tidak ada rute";
        case RH_ROUTER_ERROR_TIMEOUT:         return "TIMEOUT(3)";
        case RH_ROUTER_ERROR_NO_REPLY:        return "NO_REPLY(4)";
        case RH_ROUTER_ERROR_UNABLE_TO_DELIVER: return "UNABLE_TO_DELIVER(5) next hop tidak ACK";
        default:                              return "UNKNOWN";
      }
    }

    void dumpRoutes(const char* when) {
      Serial.print("["); Serial.print(NODE_TAG); Serial.print("][ROUTE] ");
      Serial.println(when);
      manager->printRoutingTable();
    }
};

#endif