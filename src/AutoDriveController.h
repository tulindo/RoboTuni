#ifndef _AUTODRIVE_CONTROLLER_H
#define _AUTODRIVE_CONTROLLER_H

#include "serial.h"

#include "MotorsController.h"
#include "ServoController.h"
#include "RobotCommands.h"

#include <Arduino.h>
#include <Ticker.h>

//This enum defines how the robot recovers from a dangerous situation 
//I.e. What do do When the method setDangerMode has been called (after having stopped)
enum DangerRecoveryModeEnum : byte {  
    //Standard Revocery: Backward for 1 second then do a U-Turn (180 degrees rotation). The direction of rotation is renaom.
    STANDARD_RECOVERY = 0,  
    //Random Rotation: Like the standard mode but instead of a 180 degrees rotation do a random rotation (180 +/- 45%)
    RANDOM_ROTATION_RECOVERY = 1,
    //Servo Based Recovery: Goes backward for 1 second and stops again. Then looks left and right to ged the best direction to turn. Than turns on that direction 
    SERVO_BASED_RECOVERY = 2,
};

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

    //This enum defines how the robot recovers from a dangerous situation 
    //I.e. What do do When the method setDangerMode has been called (after having stopped)
    enum DangerRecoveryModeEnum : byte {  
      //Standard Revocery: Backward for 1 second then do a U-Turn (180 degrees rotation). The direction of rotation is renaom.
      STANDARD_RECOVERY = 0,  
      //Random Rotation: Like the standard mode but instead of a 180 degrees rotation do a random rotation (180 +/- 45%)
      RANDOM_ROTATION_RECOVERY = 1,
      //Servo Based Recovery: Goes backward for 1 second and stops again. Then looks left and right to ged the best direction to turn. Than turns on that direction 
      SERVO_BASED_RECOVERY = 2,
    } dangerRecoveryMode = STANDARD_RECOVERY;

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
    explicit AutoDriveController(MotorsController* motorsController, ServoController* servoController, DangerRecoveryModeEnum dangerRecoveryMode = STANDARD_RECOVERY);

    void start();

    void stop();

    bool getIsEnabled();

    void update();

    void setDangetMode();
};

#endif