#ifndef LIDAR_H
#define LIDAR_H

#include "Adafruit_VL53L0X.h"

class lidarModule {
  public:
    Adafruit_VL53L0X sensorLidar = Adafruit_VL53L0X();

    void begin() {
      if(!sensorLidar.begin()) {
        Serial.println("Lidar Error!");
        while(1);
      }
      Serial.println("5. LiDAR DIKENALI!");
    }

    int jarakMentah() {
      VL53L0X_RangingMeasurementData_t measure;
      sensorLidar.rangingTest(&measure, false);
      if(measure.RangeStatus != 4) {
        return measure.RangeMilliMeter;
      }
      return -1;
    }
};

#endif