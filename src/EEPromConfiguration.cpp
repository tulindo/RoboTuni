#include "EEPromConfiguration.h"

#include <EEPROM.h>

//Initialize static variables
EEPromConfiguration* EEPromConfiguration::instance = nullptr;

EEPromConfiguration::EEPromConfiguration() {
}

EEPromConfiguration* EEPromConfiguration::getInstance() {
  if (!instance) {
    //Initialize once
    instance = new EEPromConfiguration(); 
  }
  return instance;
}

void EEPromConfiguration::load() {
  EEPROM.get(0, eePromData);
}
void EEPromConfiguration::store() {
  EEPROM.put(0, eePromData);
}

bool EEPromConfiguration::getIsSoftMode() {
  return eePromData.isSoftMode != 0;
}
void EEPromConfiguration::setIsSoftMode(bool value) {
  eePromData.isSoftMode = value ? 255: 0;
}

byte EEPromConfiguration::getMinMotorsSpeed() {
  return eePromData.minMotorsSpeed;
}
void EEPromConfiguration::setMinMotorsSpeed(byte value) {
  eePromData.minMotorsSpeed = value;
}

byte EEPromConfiguration::getMaxMotorsSpeed() {
  return eePromData.maxMotorsSpeed;
}
void EEPromConfiguration::setMaxMotorsSpeed(byte value) {
  eePromData.maxMotorsSpeed = value;
}

byte EEPromConfiguration::getDriftingCorrection() {
  return eePromData.driftingCorrection;
}
void EEPromConfiguration::setDriftingCorrection(byte value) {
  eePromData.driftingCorrection = value;
}

byte EEPromConfiguration::getTimingControlTick() {
  return eePromData.timingControlTick;
}
void EEPromConfiguration::setTimingControlTick(byte value) {
  eePromData.timingControlTick = value;
}

SerialDebugEnum EEPromConfiguration::getSerialDebug() {
  return (SerialDebugEnum)eePromData.serialDebug;
}
void EEPromConfiguration::setSerialDebug(SerialDebugEnum value) {
  eePromData.serialDebug = (byte)value;  
}
