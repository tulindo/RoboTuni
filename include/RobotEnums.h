#ifndef _ROBOT_ENUMS_H
#define _ROBOT_ENUMS_H

#include <Arduino.h>

//Macro definitions used to define enums whose values can easily be translated to string
#define ENUM_VALUE(name, value, string) name = value,
#define ENUM_DEF(name, values)             \
  enum name : byte {                       \
    values(ENUM_VALUE)                     \
  };                                       \
  const char* enumToString(name value);

//RobotCommandEnum definition
//This enum contains the various robot commands
#define ROBOT_COMMAND_VALUES(V)                           \
    V(STOP,                  0, "STOP")                   \
    V(MOVE_FORWARD,          1, "MOVE_FORWARD")           \
    V(MOVE_BACKWARD,         2, "MOVE_BACKWARD")          \
    V(TURN_RIGHT,            4, "TURN_RIGHT")             \
    V(TURN_LEFT,             8, "TURN_LEFT")              \
    V(MOVE_FORWARD_RIGHT,    5, "MOVE_FORWARD_RIGHT")     \
    V(MOVE_FORWARD_LEFT,     9, "MOVE_FORWARD_LEFT")      \
    V(MOVE_BACKWARD_RIGHT,   6, "MOVE_BACKWARD_RIGHT")    \
    V(MOVE_BACKWARD_LEFT,   10, "MOVE_BACKWARD_LEFT")     \
    V(ROTATE_RIGHT,         20, "ROTATE_RIGHT")           \
    V(ROTATE_LEFT,          24, "ROTATE_LEFT")            \
    V(U_TURN_RIGHT,         34, "U_TURN_RIGHT")           \
    V(U_TURN_LEFT,          38, "U_TURN_LEFT")            \
    V(TURN_REVERSE_LEFT,    50, "TURN_REVERSE_LEFT")      \
    V(TURN_REVERSE_RIGHT,   60, "TURN_REVERSE_RIGHT")     \
    V(MOVE_FORWARD_SLOW,   200, "MOVE_FORWARD_SLOW")      \
    V(AUTO_DRIVE,           64, "AUTO_DRIVE")             \
    V(EMERGENCY_STOP,      100, "EMERGENCY_STOP")         \
    V(UNKNOWN_COMMAND,     255, "UNKNOWN_COMMAND")            

ENUM_DEF(RobotCommandEnum, ROBOT_COMMAND_VALUES)

//DangerRecoveryModeEnum definition
//How do the robot recover from a dangerous situation when autodrive
//I.e. what do do when the method setDangerMode has been called (after having stopped)
enum DangerRecoveryModeEnum : byte {  
  //Standard Revocery: Backward for 1 second then do a U-Turn (180 degrees rotation). The direction of rotation is renaom.
  STANDARD_RECOVERY = 0,  
  //Random Rotation: Like the standard mode but instead of a 180 degrees rotation do a random rotation (180 +/- 45%)
  RANDOM_ROTATION_RECOVERY = 1,
  //Servo Based Recovery: Goes backward for half a second and stops again. Then looks left and right to ged the best direction to turn. Than turns on that direction 
  SERVO_BASED_RECOVERY = 2,
};

//AutoDriveStateEnum definition
//This enum defines the various states used by the state machine during auto drive mode
#define AUTO_DRIVE_STATE_VALUES(V)     \  
  V(NormalDrive, 0, "NormalDrive")     \  
  V(Stopped,     1, "Stopped")         \   
  V(Backward,    2, "Backward")        \
  V(UTurn,       3, "UTurn")           \
  V(LookLeft,    4, "LookLeft")        \
  V(LookRight,   5, "LookRight")       \
  V(Turn,        6, "Turn")             
ENUM_DEF(AutoDriveStateEnum, AUTO_DRIVE_STATE_VALUES)

//MatrixImageEnum definition
//This enum defines the various images displayed in the matrix
#define MATRIX_IMAGE_VALUES(V)                       \  
  V(IMG_LOGO_BLE,        200, "IMG_LOGO_BLE")        \  
  V(IMG_LOGO_BLE_DOT,    201, "IMG_LOGO_BLE_DOT")    \
  V(IMG_ERROR,           202, "IMG_ERROR")           \
  V(IMG_BATTERY_0,       203, "IMG_BATTERY_0")       \
  V(IMG_BATTERY_30,      204, "IMG_BATTERY_30")      \
  V(IMG_BATTERY_60,      205, "IMG_BATTERY_60")      \
  V(IMG_BATTERY_100,     206, "IMG_BATTERY_100")     \
  V(IMG_CLEAR,           207, "IMG_CLEAR")             
ENUM_DEF(MatrixImageEnum, MATRIX_IMAGE_VALUES)

//ServoStateEnum definition
//this enum defines substates used to handle servo in servo based recovery
// MotorHandled - The servo is handled by the motors
// Forward      - The servo looks fixed forward 
// Looking      - The servo is looking left or right
// Looked       - The servo finished looking
// Resuming     - The servo is resuming normal position
// Resumed      - Resuming position completed
#define SERVO_STATE_VALUES(V)            \  
  V(MotorHandled, 0, "MotorHandled")     \  
  V(Forward,      1, "Forward")          \  
  V(Looking,      2, "Looking")          \
  V(Looked,       3, "Looked")           \
  V(Resuming,     4, "Resuming")         \
  V(Resumed,      5, "Resumed")              
ENUM_DEF(ServoStateEnum, SERVO_STATE_VALUES)

//ServoTargetEnum definition
//this enum defines the variuos target for the servo control
#define SERVO_TARGET_VALUES(V)                        \  
  V(LOOK_FORWARD,         0, "LOOK_FORWARD")          \  
  V(LOOK_FORWARD_LEFT,    1, "LOOK_FORWARD_LEFT")     \  
  V(LOOK_FORWARD_RIGHT,   2, "LOOK_FORWARD_RIGHT")    \
  V(LOOK_LEFT,            3, "LOOK_LEFT")             \
  V(LOOK_RIGHT,           4, "LOOK_RIGHT")              
ENUM_DEF(ServoTargetEnum, SERVO_TARGET_VALUES)

#endif

