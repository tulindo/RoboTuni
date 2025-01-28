#ifndef _BLUETOOTHLE_MANAGER_H
#define _BLUETOOTHLE_MANAGER_H

#define SERVICE_UUID "b66ecb16-6768-4757-952c-6bace81043b1"
#define MOTOR_CHARACTERISTIC_UUID "6f5b21cb-e0ef-442d-be02-460ca56f8542"

#include "serial.h"

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "RobotEnums.h"
#include <EEPromConfiguration.h>

class BluetoothLEManager {
  private:
    static bool isConnected;
    BLEService robotCarService; 
    static void onConnect(BLEDevice central);
    static void onDisconnect(BLEDevice central);
    BLEByteCharacteristic motorCharacteristic;
    static void onMotorCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic);
  
    //Pointer for the callback
    void (*onCommandReceivedCallback)(RobotCommandEnum);  
    static BluetoothLEManager* instance;

    //Debug internal variable
    static bool isDebug; 
  public:
    BluetoothLEManager();
    bool getIsConnected();
    void update();
    bool begin();
    //Set the callback that will be called when a command is received
    void setOnCommandReceivedCallback(void (*callback)(RobotCommandEnum));
};

#endif
