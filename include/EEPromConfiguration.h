#ifndef _EEPROM_CONFIGURATION_H
#define _EEPROM_CONFIGURATION_H

#include <Arduino.h>
#include <EEPROM.h>
#include "serial.h"

class EEPromConfiguration {
  private:
    //Internal Data Structure that will be persisted on EEPROM
    struct EEPromData {
      byte isSoftMode = 255;
      byte minMotorsSpeed = 100;
      byte maxMotorsSpeed = 150; //210;
      byte driftingCorrection = 0;
      byte timingControlTick = 20;
    } eePromData;
  public:
    //Constructor
    explicit EEPromConfiguration();

    //Load config from EEPROM
    void load();

    //Store current Config on EEPROM
    void store();

    //SoftMode
    bool getIsSoftMode();
    void setIsSoftMode(bool value);

    //Minimum speed for Motors (setting speed below this value motor will make noise and not move at all)
    byte getMinMotorsSpeed();
    void setMinMotorsSpeed(byte value);

    //Maximum speed for Motors (setting speed above this value will give too much voltage to the motor)
    //Default is 255
    byte getMaxMotorsSpeed();
    void setMaxMotorsSpeed(byte value);

    //Pulse correction in order to keep the robot go straight ahead instead of turning left or right 
    //Value is a % that goes from -100 to 100. Default is 0 meaning that robot goes straight without corrections.
    //Positive values means that the Robot tends to move right instead of going straight. So give more power to the DX motor compared to DX-
    //I.e. if speed is 200 and Correction is +10% ==> DX motor will receive a pulse of 220 instead of 200 given to SX motor.
    byte getDriftingCorrection();
    void setDriftingCorrection(byte value);

    //Motor adjustment speed timer delay
    byte getTimingControlTick();
    void setTimingControlTick(byte value);
};

#endif