#ifndef LIDAR_H
#define LIDAR_H

#include <Wire.h>
#include <VL53L1X.h>

class lidarModule {
  public:
    VL53L1X sensorLidar;

    void begin() {
      // Mengatur batas waktu tunggu agar sistem tidak hang/freeze
      sensorLidar.setTimeout(500);
      
      if (!sensorLidar.init()) {
        Serial.println("Lidar Error!");
        while (1);
      }
      Serial.println("5. LiDAR DIKENALI!");
      
      sensorLidar.setDistanceMode(VL53L1X::Short);
      sensorLidar.setROISize(4, 4);
      sensorLidar.setMeasurementTimingBudget(50000); 
      sensorLidar.startContinuous(50);
    }

    int jarakMentah() {
      int distance = sensorLidar.read();
      
      if (sensorLidar.timeoutOccurred()) {
        return -1; 
      }
      
      return distance;
    }
};

#endif