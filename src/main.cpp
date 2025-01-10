#include <Arduino.h>

//Include serial defines
#include "serial.h"

//EEProm configuration object
#include "EEPromConfiguration.h"
EEPromConfiguration eePromConfig;

//Matrix controller object
#include "MatrixController.h"
MatrixController matrix;

//Motors control
//Motor controlling pins
#define PIN_RIGHT_FORWARD 8
#define PIN_RIGHT_BACKWARD 9
#define PIN_RIGHT_SPEED 10
#define PIN_LEFT_FORWARD 13
#define PIN_LEFT_BACKWARD 12
#define PIN_LEFT_SPEED 11

#include <MotorsController.h>
MotorsController motorsController = MotorsController(
  PIN_LEFT_SPEED, 
  PIN_LEFT_FORWARD, 
  PIN_LEFT_BACKWARD, 
  PIN_RIGHT_SPEED, 
  PIN_RIGHT_FORWARD, 
  PIN_RIGHT_BACKWARD);

//Servo Controller
#include "ServoController.h"
#define SERVO_PIN 3
#define TIMER_DELAY 41

ServoController servoController(SERVO_PIN, TIMER_DELAY);

#include <Ticker.h>

//Front Distance Sensor (Trigger and Echo pin are shared)
#include "DistanceSensor.h"
#define TRIGGER_PIN 2
#define ECHO_PIN 2
#define MAX_DISTANCE 100
#define SAFE_DISTANCE_CM 20

bool isUnsafeDistanceFront = false;

DistanceSensor distanceSensor(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

//This timer handles the motors by calling every 20ms the update function of the motorsController
Ticker timerMotorCommand(
  []() { motorsController.update(); }, 
    20, 0, MILLIS);

//Rear distance sensors
#include "InfraredSensors.h"

#define PIN_LEFT_REAR_SENSOR 7
#define PIN_RIGHT_REAR_SENSOR 5

bool isUnsafeDistanceBack = false;

InfraredSensors rearSensors(PIN_LEFT_REAR_SENSOR, PIN_RIGHT_REAR_SENSOR);

//Auto Drive controller
#include <AutoDriveController.h>
AutoDriveController autoDriveController(&motorsController, &servoController, &distanceSensor);

//Buzzer
#define PIN_BUZZER 6
#define BUZZ_FREQUENCY 440

#include "Buzzer.h"

Buzzer buzzer(PIN_BUZZER);

//Bluetooth® Low Energy handling
#include "BluetoothLEManager.h"
#include <ArduinoBLE.h>

BluetoothLEManager bleManager;

void danger() {
  buzzer.buzz(BUZZ_FREQUENCY);
  matrix.showCommand(EMERGENCY_STOP);
  motorsController.emergencyStop();
  if (autoDriveController.getIsEnabled()) {
    autoDriveController.setDangerMode();
  }
}

//Callback called when a command from BLE is received
void myCommandReceivedCallback(RobotCommandEnum command) {
  // SerialPrint("Received command: ");
  // SerialPrintln(command);
  if ((!isUnsafeDistanceFront || motorsController.isSafeCommandFront(command)) &&
      (!isUnsafeDistanceBack || motorsController.isSafeCommandBack(command))) {
    //It's safe to execute the received command
    matrix.showCommand(command);
    if (command == AUTO_DRIVE) {
      //Start Automatic drive mode
      autoDriveController.start(SERVO_BASED_RECOVERY);
    } else {
      //Stop automatic drive mode if it was enabled
      if (autoDriveController.getIsEnabled()) {
        autoDriveController.stop();
      }
      //Execute command
      motorsController.execute(command);
    }
  } else {
    //Signal that the received command is unsafe
    buzzer.buzz(BUZZ_FREQUENCY);
    matrix.showCommand(EMERGENCY_STOP);
  }
}

//Callback called when a motor command is Executed
void myCommandExecutedCallback(RobotCommandEnum command) {
  // SerialPrint("Executed command: ");
  // SerialPrintln(command);
  //Handle Oscillation Servo Controller
  switch (command) {
    case MOVE_FORWARD:
    case MOVE_FORWARD_SLOW:
      servoController.setMode(LOOK_FORWARD, true);
      break;
    case MOVE_FORWARD_LEFT:
      servoController.setMode(LOOK_FORWARD_LEFT, true);
      break;
    case MOVE_FORWARD_RIGHT:
      servoController.setMode(LOOK_FORWARD_RIGHT, true);
      break;
    case TURN_LEFT:
    case ROTATE_LEFT:
    case U_TURN_LEFT:
      servoController.setMode(LOOK_LEFT);
      break;
    case TURN_RIGHT:
    case ROTATE_RIGHT:
    case U_TURN_RIGHT:
      servoController.setMode(LOOK_RIGHT);
      break;
    default:
      servoController.setMode(LOOK_FORWARD);
  }
}

//Main Timer. This timer ticks every 50ms.
//It does two things (one each time)
//1. BLE Commands handling 
//2. front distance measurement with HC-SR04
//I choosed to use a single timer because, beind the two operations blocking this guarantees a better distribution of blocking operations

bool isBleTimingMode; //True if the timer is polling for BLE, false if it is in the distance measuring mode.

void mainTimer() {

  if (isBleTimingMode) {
    //BLE polling
    bleManager.update();   
  } else {
    //Front Distance measuring
    distanceSensor.distanceBlocking();
    isUnsafeDistanceFront = distanceSensor.getDistance() < SAFE_DISTANCE_CM;
    if (isUnsafeDistanceFront && !motorsController.isSafeCommandFront()) {
      //Immediately stop the motor for safety reason
      danger();
    } 
  }

  //Rear Distance
  isUnsafeDistanceBack = rearSensors.obstacleDetected();
  if (isUnsafeDistanceBack && !motorsController.isSafeCommandBack()) {
      //Immediately stop the motor for safety reason
      danger();
  }
  isBleTimingMode = !isBleTimingMode;
}

Ticker timerMain(mainTimer, 50, 0, MILLIS );

//Gestione blinking Timer
//Questo Timer si occupa di blinkare un led della matrice quando si è in attesa di connessione
bool blinkFlag = false;

void blinkBleTimer() {
    matrix.showImage(blinkFlag ? IMG_LOGO_BLE : IMG_LOGO_BLE_DOT);
    blinkFlag = !blinkFlag;
}
Ticker timerBlinkBle(blinkBleTimer, 1000, 0, MILLIS);

void setup() { 
  Serial.begin(115200);

  matrix.begin();
  matrix.showImage(IMG_BATTERY_0);

  //Initialize Serial
  while (!Serial);
  Serial.println("Serial communication initialized");
  delay(1000);

  matrix.showImage(IMG_BATTERY_30);
  //Init configuration
  motorsController.inizializeSpeeds(eePromConfig);
  delay(1000);

  matrix.showImage(IMG_BATTERY_60);

  //Configure callbacks
  bleManager.setOnCommandReceivedCallback(myCommandReceivedCallback);
  motorsController.setOnCommandExecutedCallback(myCommandExecutedCallback);
 
  delay(1000);

  if (!bleManager.begin()) {
    //Show error image and stop
    matrix.showImage(IMG_ERROR);
    while(true);
  }

  matrix.showImage(IMG_BATTERY_100);
  delay(1000);

  //Start Main Timer handling BLE and HC_SR04
  timerMain.start();
  //start the blink timer
  timerBlinkBle.start();
  //Start timer for soft motor command control
  timerMotorCommand.start();

  //Inizialize Servo Controller 
  servoController.begin();

  //Clear the matrix
  matrix.showImage(IMG_CLEAR);
  //Buzz to signal that restart is completed
  buzzer.buzz(BUZZ_FREQUENCY);
}

void loop() {  
  matrix.update();
  buzzer.update();
  timerMain.update();
  timerMotorCommand.update();
  autoDriveController.update();
  servoController.update();

  if (!bleManager.getIsConnected() && !isUnsafeDistanceFront && !isUnsafeDistanceBack) {  
    //Blink only if the dustance if safe, otherwise signal the danger
    timerBlinkBle.update();  
  }
}