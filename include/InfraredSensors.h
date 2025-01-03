#ifndef _INFRARED_SENSORS_H
#define _INFRARED_SENSORS_H

#include "serial.h"

#include <Arduino.h>

class InfraredSensors {
  private:
    int pinLeft;
    int pinRight;
    
  public:

    //Constructor
    explicit InfraredSensors(
      int pinLeft, 
      int pinRight);

    //Check if one of the two sensor found something 
    bool obstacleDetected();

};

#endif