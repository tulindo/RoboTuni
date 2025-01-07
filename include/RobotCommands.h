#ifndef _ROBOT_COMMANDS_H
#define _ROBOT_COMMANDS_H

#include <Arduino.h>

//The various motor commands implemented by the robot
enum RobotCommandEnum : byte {  
  STOP = 0, 
  MOVE_FORWARD = 1, 
  MOVE_BACKWARD = 2,
  TURN_RIGHT = 4,
  TURN_LEFT = 8,
  MOVE_FORWARD_RIGHT = 5,
  MOVE_FORWARD_LEFT = 9,
  MOVE_BACKWARD_RIGHT = 6,
  MOVE_BACKWARD_LEFT = 10,
  ROTATE_RIGHT = 20,
  ROTATE_LEFT = 24,
  U_TURN_RIGHT = 34,
  U_TURN_LEFT = 38,
  TURN_REVERSE_LEFT = 50,
  TURN_REVERSE_RIGHT = 60,
  MOVE_FORWARD_SLOW = 200,
  AUTO_DRIVE = 64,
  EMERGENCY_STOP = 100,
  UNKNOWN_COMMAND = 255
};

//How do the robot recover from a dangerous situation when autodrive
//I.e. what do do when the method setDangerMode has been called (after having stopped)
enum DangerRecoveryModeEnum : byte {  
  //Standard Revocery: Backward for 1 second then do a U-Turn (180 degrees rotation). The direction of rotation is renaom.
  STANDARD_RECOVERY = 0,  
  //Random Rotation: Like the standard mode but instead of a 180 degrees rotation do a random rotation (180 +/- 45%)
  RANDOM_ROTATION_RECOVERY = 1,
  //Servo Based Recovery: Goes backward for 1 second and stops again. Then looks left and right to ged the best direction to turn. Than turns on that direction 
  SERVO_BASED_RECOVERY = 2,
};

#endif

