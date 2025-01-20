#include "RobotEnums.h"

//Macro definition to implement the enumToString function 
#define ENUM_CASE(name, value, string) case name: return F(string);
#define ENUM_IMPL(name, values)            \
  const __FlashStringHelper* enumToString(name value) {   \
    switch (value) {                       \
      values(ENUM_CASE)                    \
      default: return F("Unknown");           \
    }                                      \
  }

//enumToString implementation for the various enums
ENUM_IMPL(RobotCommandEnum, ROBOT_COMMAND_VALUES)
ENUM_IMPL(AutoDriveStateEnum, AUTO_DRIVE_STATE_VALUES)
ENUM_IMPL(MatrixImageEnum, MATRIX_IMAGE_VALUES)
ENUM_IMPL(ServoStateEnum, SERVO_STATE_VALUES)
ENUM_IMPL(ServoTargetEnum, SERVO_TARGET_VALUES)
