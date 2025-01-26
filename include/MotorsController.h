#ifndef _MOTORS_CONTROLLER_H
#define _MOTORS_CONTROLLER_H

#include "serial.h"

#include "EEPromConfiguration.h"
#include "RobotEnums.h"
#include "Queue.h"

#include <Arduino.h>
#include <Ticker.h>
#include <L298N.h>

#define U_TURN_TICKS 46

class MotorsController {
  private:
    short currentLeftMotorSpeed = 0;
    short currentRightMotorSpeed = 0;
    short targetLeftMotorSpeed = 0;
    short targetRightMotorSpeed = 0;

    short maxSpeed = 0;
    short slowSpeed = 0;
    short minSpeed = 0;
    short safeMaxSpeed = 0;
    short safeSlowSpeed = 0;

    RobotCommandEnum currentCommand = UNKNOWN_COMMAND;
    L298N leftMotor;
    L298N rightMotor;
    Queue queueSpeeds;
    short leftSpeedsArray[5];
    short rightSpeedsArray[5];
    void moveMotors(short leftSpeed, short rightSpeed, bool isSoft = false);
    void setMotorsTargetSpeed(RobotCommandEnum command, short leftSpeed, short rightSpeed, int motorTicks = 1);
    byte calculateMotorSpeeds(short speeds[], short currentTargetSpeed, short newTargetSpeed);
    
    //Pointer for the callback
    void (*onCommandExecutedCallback)(RobotCommandEnum); 
    static MotorsController* instance;

    //Debug internal variable
    static bool isDebug; 
  public:

    //Constructor
    explicit MotorsController(
      int pinSpeedLeft, 
      int pinForwardLeft, 
      int pinBackwardLeft,
      int pinSpeedRight,
      int pinForwardRight,
      int pinBacwardRight);

    //Initialize speeds given the configuration
    void begin(EEPromConfiguration configuration);

    //Emergency stop
    void emergencyStop();

    //This method drive the motors using the timing
    //Every command can have an optional parameter
    void execute(RobotCommandEnum cmd, short parameter = 0);

    //Main timer function
    void update();

    //SafetyMethods
    bool isSafeCommandFront(RobotCommandEnum cmd = UNKNOWN_COMMAND );
    bool isSafeCommandBack(RobotCommandEnum cmd = UNKNOWN_COMMAND);

    //Set the callback that will be called when a command is executed
    void setOnCommandExecutedCallback(void (*callback)(RobotCommandEnum));
};

#endif