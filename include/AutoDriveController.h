#ifndef _AUTODRIVE_CONTROLLER_H
#define _AUTODRIVE_CONTROLLER_H

#include "serial.h"

#include "MotorsController.h"
#include "ServoController.h"
#include "DistanceSensor.h"
#include "RobotEnums.h"

#include <Arduino.h>
#include <Ticker.h>

class AutoDriveController {
  private:
    MotorsController* motorsController = nullptr;
    ServoController* servoController = nullptr;
    DistanceSensor* distanceSensor = nullptr;

    bool isEnabled;
    //Current state of the controller
    AutoDriveStateEnum state = NormalDrive;
    //Curent state of the Servo
    ServoStateEnum servoState = MotorHandled;

    DangerRecoveryModeEnum dangerRecoveryMode;
    struct CommandData {
      RobotCommandEnum command; 
      byte percentage;
      short minDurationMillis;
      short maxDurationMillis;
    } commands[7];

    Ticker timer;
    void onTick();

    static AutoDriveController* instance;
    void onServoTargetReached(ServoTargetEnum target);

    float minLeftDistance;
    float maxLeftDistance;
    float minRightDistance;
    float maxRightDistance;

    //Helper methods for the onTick
    void onTickNormalDrive();
    void onTickUTurn();
    void onTickLookSide(bool isRight);

  public:
    //Constructor
    explicit AutoDriveController(MotorsController* motorsController, ServoController* servoController, DistanceSensor* distanceSensor);

    void start(DangerRecoveryModeEnum mode);

    void stop();

    bool getIsEnabled();

    void update();

    void setDangerMode();

    static void myServoTargetReceivedCallback(ServoTargetEnum target); 
};

#endif