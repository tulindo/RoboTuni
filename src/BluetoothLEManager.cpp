#include "BluetoothLEManager.h"

//Initialize static variables
BluetoothLEManager* BluetoothLEManager::instance = nullptr;
bool BluetoothLEManager::isConnected = false;
bool BluetoothLEManager::isDebug = false;

BluetoothLEManager::BluetoothLEManager() :
  robotCarService(SERVICE_UUID),
  motorCharacteristic(MOTOR_CHARACTERISTIC_UUID, BLEWrite),
  onCommandReceivedCallback(nullptr) {
    instance = this;
}

bool BluetoothLEManager::getIsConnected() {
  return isConnected;
}

bool BluetoothLEManager::begin(EEPromConfiguration configuration) {
  isDebug = configuration.getSerialDebug() & SerialDebugEnum::DebugBluetoothLEManager;

  //Initialize BLE module
  if (!BLE.begin()) { 
    SerialPrintln("starting Bluetooth® Low Energy module failed!");
    return false;
  }

  SerialPrintln("BLE correctly inizialized");

  //Set name and uuid for RoboTuni Service
  BLE.setLocalName("RoboTuni");
  BLE.setAdvertisedService(robotCarService);

  //Add characteristics
  robotCarService.addCharacteristic(motorCharacteristic);
  
  //Publish the service
  BLE.addService(robotCarService);

  //Event handlers 
  BLE.setEventHandler(BLEConnected, onConnect);
  BLE.setEventHandler(BLEDisconnected, onDisconnect);
  motorCharacteristic.setEventHandler(BLEWritten, onMotorCharacteristicWritten);

  BLE.advertise();

  SerialPrintln("BLE Configured");
  return true;
}

void BluetoothLEManager::update() {
  //Poll for BLE events
  BLE.poll();
}

//Set the user callback to be called when a command has been received
void BluetoothLEManager::setOnCommandReceivedCallback(void (*callback)(RobotCommandEnum)) {
    onCommandReceivedCallback = callback;
}

//Central connected event handler
void BluetoothLEManager::onConnect(BLEDevice central) {
  SerialPrint("Connected event, central: ");
  SerialPrintln(central.address());
  isConnected = true;
}

//Central disconnected eventi handler
void BluetoothLEManager::onDisconnect(BLEDevice central) {
  // central disconnected event handler
  SerialPrint("Disconnected event, central: ");
  SerialPrintln(central.address());
  isConnected = false;
}

//Motor characteristic written event handler
void BluetoothLEManager::onMotorCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  //Read the received command
  byte receivedCommand;
  characteristic.readValue(receivedCommand);
  //Check if the received command is valid
  switch (receivedCommand) {
    case STOP:
    case MOVE_FORWARD: 
    case MOVE_BACKWARD:
    case TURN_RIGHT:
    case TURN_LEFT:
    case MOVE_FORWARD_RIGHT:
    case MOVE_FORWARD_LEFT:
    case MOVE_BACKWARD_RIGHT:
    case MOVE_BACKWARD_LEFT:
    case ROTATE_RIGHT:
    case ROTATE_LEFT:
    case U_TURN_RIGHT:
    case U_TURN_LEFT:
    case AUTO_DRIVE:
      break;
    default:
      //Set the received command as unknnown
      SerialPrint("Unknown byte received from central: ");
      SerialPrintln(receivedCommand);
      receivedCommand = UNKNOWN_COMMAND;
    break;
  }

  //Call configured callback with received command as argument
  if (instance->onCommandReceivedCallback) {
    instance->onCommandReceivedCallback((RobotCommandEnum)receivedCommand);
  } else {
    SerialPrintln("Command Received Callback not set");
  }
}