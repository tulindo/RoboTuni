#ifndef _AUTODRIVE_CONTROLLER_H
#define _AUTODRIVE_CONTROLLER_H

#include "serial.h"

#include "MotorsController.h"
#include "ServoController.h"
#include "RobotCommands.h"

#include <Arduino.h>
#include <Ticker.h>

class AutoDriveController {
  private:
    MotorsController* motorsController = nullptr;
    ServoController* servoController = nullptr;
    bool isEnabled;

    //This enum defines the various states used by the stata machine
    enum AutoDriveStateEnum : byte {
      //Normal drive mode.
      NormalDrive,
      //This is the first state when entered in the recovery mode (i.e. when the setDangerMode method has been called)
      Stopped,
      //Move backward
      Backward,
      //Do a U-Turn rotation
      UTurn,
      //LookLeft to get the maximum distance
      LookLeft,
      //Look right to get the maximum distance
      LookRight,
      //Turn
      Turn
    } state = NormalDrive;

    DangerRecoveryModeEnum dangerRecoveryMode;
    
    struct CommandData {
      RobotCommandEnum command; 
      byte percentage;
      short minDurationMillis;
      short maxDurationMillis;
    } commands[7];

    Ticker timer;
    void onTick();

  public:
    //Constructor
    explicit AutoDriveController(MotorsController* motorsController, ServoController* servoController);

    void start(DangerRecoveryModeEnum mode);

    void stop();

    bool getIsEnabled();

    void update();

    void setDangerMode();
};

#endif