#include "MotorsController.h"

//Initialize static variables (otherwise linker will fail)
MotorsController* MotorsController::instance = nullptr;

MotorsController::MotorsController(
    int pinSpeedLeft, 
    int pinForwardLeft, 
    int pinBackwardLeft,
    int pinSpeedRight,
    int pinForwardRight,
    int pinBacwardRight) :
  leftMotor(pinSpeedLeft, pinForwardLeft, pinBackwardLeft), 
  rightMotor(pinSpeedRight, pinForwardRight, pinBacwardRight),
  queueSpeeds(),
  onCommandExecutedCallback(nullptr) {
    instance = this;
}

void MotorsController::moveMotors(short leftSpeed, short rightSpeed, bool isSoft) {
  if (leftMotor.getSpeed() != abs(leftSpeed) || 
      rightMotor.getSpeed() != abs(rightSpeed)) {
    //Motors speed changed, so apply it
    // SerialPrint(isSoft ? "Adjusting" : "Setting");
    // SerialPrint(" speed: (");
    // SerialPrint(leftSpeed);
    // SerialPrint(",");
    // SerialPrint(rightSpeed);
    // SerialPrintln(")");

    leftMotor.setSpeed( leftSpeed >=0 ? leftSpeed : - leftSpeed);
    if (leftSpeed >=0) {
      leftMotor.forward();
    } else {
      leftMotor.backward();
    }
    rightMotor.setSpeed( rightSpeed >=0 ? rightSpeed : - rightSpeed);
    if (rightSpeed >=0) {
      rightMotor.forward();
    } else {
      rightMotor.backward();
    }
  }
  currentLeftMotorSpeed = leftSpeed;
  currentRightMotorSpeed = rightSpeed;
  if (!isSoft) {
    queueSpeeds.clear();
    targetLeftMotorSpeed = currentLeftMotorSpeed;
    targetRightMotorSpeed = currentRightMotorSpeed;
  }
}

byte MotorsController::calculateMotorSpeeds(short speeds[], short currentTargetSpeed, short newTargetSpeed) {
  byte idx=0;
  if (newTargetSpeed > currentTargetSpeed) {
    //Should increase Speed
    // SerialPrint("Increase: ");
    if (newTargetSpeed <= -minSpeed) {
      //Desired speed if below backward min then immediately set desired speed 
      speeds[idx++] = newTargetSpeed;
      // SerialPrint(newTargetSpeed);
      return idx;
    }
    if (currentTargetSpeed < -minSpeed) {
      // SerialPrint(-minSpeed);
      // SerialPrint(" ");
      speeds[idx++] = -minSpeed;
    }
    if (currentTargetSpeed < 0) {
      // SerialPrint("0 ");
      speeds[idx++] = 0;
    }
    if (newTargetSpeed == 0) {
      return idx;
    }
    if (currentTargetSpeed < minSpeed) {
      // SerialPrint(minSpeed);
      // SerialPrint(" ");
      speeds[idx++] = minSpeed;
    }
    if (newTargetSpeed == minSpeed) {
      return idx;
    }
    // SerialPrint(newTargetSpeed);
    speeds[idx++] = newTargetSpeed;
  } else if (newTargetSpeed < currentTargetSpeed) {
    //Should decrease Speed
    // SerialPrint("Decrease: ");
    if (newTargetSpeed >= minSpeed) {
      //Desired speed if above forward min then immediately set desired speed
      // SerialPrint(newTargetSpeed);
      speeds[idx++] = newTargetSpeed;
      return idx;
    }
    if (currentTargetSpeed > minSpeed) {
      // SerialPrint(minSpeed);
      // SerialPrint(" ");
      speeds[idx++] = minSpeed;
    }
    if (currentTargetSpeed > 0) {
      // SerialPrint("0 ");
      speeds[idx++] = 0;
    }
    if (newTargetSpeed == 0) {
      return idx;
    }
    if (currentTargetSpeed > -minSpeed) {
      // SerialPrint(-minSpeed);
      // SerialPrint(" ");
      speeds[idx++] = -minSpeed;
    }
    if (newTargetSpeed == -minSpeed) {
      return idx;
    }
    // SerialPrint(newTargetSpeed);
    speeds[idx++] = newTargetSpeed;
  } else {
    // SerialPrint("Keep: ");
    // SerialPrint(newTargetSpeed);
    speeds[idx++] = newTargetSpeed;
  }

  return idx;
} 

void MotorsController::setMotorsTargetSpeed(RobotCommandEnum command, short leftSpeed, short rightSpeed, int motorTicks) {
  // SerialPrint("Speed : (");
  // SerialPrint(currentLeftMotorSpeed);
  // SerialPrint(",");
  // SerialPrint(currentRightMotorSpeed);
  // SerialPrint(") -> (");
  // SerialPrint(leftSpeed);
  // SerialPrint(",");
  // SerialPrint(rightSpeed);
  // SerialPrint(") LEFT (");
  int numLeftSteps = calculateMotorSpeeds(leftSpeedsArray, targetLeftMotorSpeed, leftSpeed);
  // SerialPrint(") RIGHT (");
  int numRightSteps = calculateMotorSpeeds(rightSpeedsArray, targetRightMotorSpeed, rightSpeed);
  // SerialPrintln(")");
  byte steps = max(numLeftSteps, numRightSteps);
  for (byte idx = 0; idx < steps; idx++) {
    short ls = leftSpeedsArray[min(idx, numLeftSteps-1)];
    short rs = rightSpeedsArray[min(idx, numRightSteps-1)];
    queueSpeeds.enqueue(QueueData(command, ls, rs), idx == steps -1 ? motorTicks : 1 );
  }
  targetLeftMotorSpeed = leftSpeed;
  targetRightMotorSpeed = rightSpeed;
}

void MotorsController::inizializeSpeeds(EEPromConfiguration configuration) {
  //Min And Max speed are read from configuration
  minSpeed = (short)configuration.getMinMotorsSpeed();
  maxSpeed = (short)configuration.getMaxMotorsSpeed();
  //SlowSpeed is used to forward turns. It's one third of the max speed .
  slowSpeed = minSpeed + (maxSpeed-minSpeed)/3;
  //Like a real car robot will go faster moving forward than backward.
  //This is also for safety reason because rear distance sensors are only proximity sensors
  //They signal obstables only when they are  very close. The car has to stop in a shorter distance.
  //This is achieved going slow.
  //Safe Max is the backward speed and it one half of the forward speed.
  safeMaxSpeed = minSpeed + (maxSpeed-minSpeed)/2;
  //Even going backward slow speed is one third of the max
  safeSlowSpeed = minSpeed + (safeMaxSpeed-minSpeed)/3;
}

void MotorsController::emergencyStop() {
  //Immediately stop motors
  moveMotors(0, 0);
  //This give time to the motor for really stopping
  queueSpeeds.enqueue(QueueData(STOP), 20);
}

void MotorsController::execute(RobotCommandEnum cmd, short parameter) {
  //SerialPrint("MotorsController Execute: ");
  // SerialPrint(enumToString(cmd));
  if (parameter != 0) {
    // SerialPrint(" (");
    // SerialPrint(parameter);
    // SerialPrintln(")");
  } else {
    //SerialPrintln();
  }
  switch (cmd) {
    case STOP:
      setMotorsTargetSpeed(STOP, 0, 0);
      break;
    case MOVE_FORWARD:
      setMotorsTargetSpeed(MOVE_FORWARD, maxSpeed, maxSpeed);
      break;
    case MOVE_BACKWARD:
      setMotorsTargetSpeed(MOVE_BACKWARD, -safeMaxSpeed, -safeMaxSpeed);
      break;
    case TURN_LEFT:
      if (currentCommand == MOVE_BACKWARD || currentCommand == MOVE_BACKWARD_LEFT || currentCommand == MOVE_BACKWARD_RIGHT)
      {
        setMotorsTargetSpeed(TURN_REVERSE_LEFT, 0, -maxSpeed);
      } else {
        setMotorsTargetSpeed(TURN_LEFT, 0, maxSpeed);
      }
      break;
    case TURN_RIGHT:
      if (currentCommand == MOVE_BACKWARD || currentCommand == MOVE_BACKWARD_LEFT || currentCommand == MOVE_BACKWARD_RIGHT)
      {
        setMotorsTargetSpeed(TURN_REVERSE_RIGHT, -maxSpeed, 0);
      } else {
        setMotorsTargetSpeed(TURN_RIGHT, maxSpeed, 0);
      }
      break;
    case MOVE_FORWARD_LEFT:
      setMotorsTargetSpeed(MOVE_FORWARD_LEFT, slowSpeed, maxSpeed);
      break;
    case MOVE_FORWARD_RIGHT:
      setMotorsTargetSpeed(MOVE_FORWARD_RIGHT, maxSpeed, slowSpeed);
      break;
    case MOVE_BACKWARD_LEFT:
      setMotorsTargetSpeed(MOVE_BACKWARD_LEFT, -safeSlowSpeed, -safeMaxSpeed);
      break;
    case MOVE_BACKWARD_RIGHT:
      setMotorsTargetSpeed(MOVE_BACKWARD_RIGHT, -safeMaxSpeed, -safeSlowSpeed);
      break;
    case ROTATE_LEFT:
      setMotorsTargetSpeed(ROTATE_LEFT, -safeMaxSpeed, safeMaxSpeed);
      break;
    case ROTATE_RIGHT:
      setMotorsTargetSpeed(ROTATE_RIGHT, safeMaxSpeed, -safeMaxSpeed);
      break;
    case U_TURN_LEFT:
      //Rotate for U_TURN_TICKS motor ticks
      //For U_TURN commands the optional parameter indicates a delta % in number of ticks
      setMotorsTargetSpeed(U_TURN_LEFT, -safeSlowSpeed, safeSlowSpeed, U_TURN_TICKS + U_TURN_TICKS * parameter / 100);
      setMotorsTargetSpeed(MOVE_FORWARD_SLOW, safeSlowSpeed, safeSlowSpeed);
      break;
    case U_TURN_RIGHT:
      //Rotate for U_TURN_TICKS motor ticks
      //For U_TURN commands the optional parameter indicates a delta % in number of ticks
      setMotorsTargetSpeed(U_TURN_RIGHT, safeSlowSpeed, -safeSlowSpeed, U_TURN_TICKS + U_TURN_TICKS * parameter / 100);
      setMotorsTargetSpeed(MOVE_FORWARD_SLOW, safeSlowSpeed, safeSlowSpeed);
      break;
  }
}

//Adjust current motors speed to reach target
void MotorsController::update() {
  if (!queueSpeeds.isEmpty()) {
    QueueData data = queueSpeeds.dequeue();
    if (data.command != currentCommand) {
      currentCommand = data.command;
      //Call configured callback with the executed command as argument
      if (instance->onCommandExecutedCallback) {
        instance->onCommandExecutedCallback(currentCommand);
      } else {
        SerialPrintln("Command Executed Callback not set");
      }
    }
    //Set motors speed    
    currentLeftMotorSpeed = data.leftMotorSpeed;
    currentRightMotorSpeed = data.rightMotorSpeed;
    moveMotors(currentLeftMotorSpeed, currentRightMotorSpeed, true);
  }
}

bool MotorsController::isSafeCommandFront(RobotCommandEnum cmd) {
  if (cmd == UNKNOWN_COMMAND) {
    cmd = currentCommand;
  }
  return
    cmd != MOVE_FORWARD_SLOW &&
    cmd != MOVE_FORWARD &&
    cmd != MOVE_FORWARD_LEFT &&
    cmd != MOVE_FORWARD_RIGHT &&
    cmd != TURN_LEFT && 
    cmd != TURN_RIGHT;
}

bool MotorsController::isSafeCommandBack(RobotCommandEnum cmd) {
  if (cmd == UNKNOWN_COMMAND) {
    cmd = currentCommand;
  }
  return
    cmd != MOVE_BACKWARD &&
    cmd != MOVE_BACKWARD_LEFT &&
    cmd != MOVE_BACKWARD_RIGHT &&
    cmd != TURN_REVERSE_LEFT &&
    cmd != TURN_REVERSE_RIGHT &&
    cmd != ROTATE_LEFT &&
    cmd != ROTATE_RIGHT;
}

//Set the user callback to be called when a command has been received
void MotorsController::setOnCommandExecutedCallback(void (*callback)(RobotCommandEnum)) {
  onCommandExecutedCallback = callback;
}
