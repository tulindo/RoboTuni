#include "AutoDriveController.h"

//Initialize static variables (otherwise linker will fail)
AutoDriveController* AutoDriveController::instance = nullptr;

AutoDriveController::AutoDriveController(
    MotorsController* motorsController, 
    ServoController* servoController,
    DistanceSensor* distanceSensor) :
  motorsController(motorsController),
  servoController(servoController),
  distanceSensor(distanceSensor),
  timer([this]() { onTick(); }, 1000, 0, MILLIS) {
    instance = this;
    dangerRecoveryMode = STANDARD_RECOVERY;
    isEnabled = false;
    commands[0] = { MOVE_FORWARD, 30, 2000, 4000 };
    commands[1] = { TURN_LEFT, 15, 1000, 2000 };
    commands[2] = { TURN_RIGHT, 15, 1000, 2000 };
    commands[3] = { MOVE_FORWARD_LEFT, 10, 1000, 2000 };
    commands[4] = { MOVE_FORWARD_RIGHT, 10, 1000, 2000 };
    commands[5] = { ROTATE_LEFT, 10, 1000, 2500 };
    commands[6] = { ROTATE_RIGHT, 10, 1000, 2500 };
}

void AutoDriveController::onTickNormalDrive() {
  servoState = MotorHandled;
  //Get a target from 1 to 100
  int target = random(1, 101);
  //Preselect first command
  CommandData command = commands[0];
  int current = command.percentage;
  if (current < target) {
    //Eventually look for others
    for (int i=1; i<7; i++) {
      //Find first command based on random
      current += commands[i].percentage;
      if (current >= target) {
        //Command Found
        command = commands[i];
        break;
      } 
    }
  }
  //Execute chosen command
  motorsController->execute(command.command);
  //Adjust timer interval based on configuration
  timer.interval(random(command.minDurationMillis, command.maxDurationMillis));
}

void AutoDriveController::onTickUTurn() {
  timer.interval(3000);
  int delta = 0;
  if (dangerRecoveryMode == RANDOM_ROTATION_RECOVERY) {
    delta = random(-45, 45);
  }
  //Default is random direction
  RobotCommandEnum uTurnCommand = random(2) == 0 ? U_TURN_LEFT : U_TURN_RIGHT;
  if (dangerRecoveryMode == SERVO_BASED_RECOVERY) {
    //Direction to turn is chosed by the min and max distances reported by the servo
    delta = -45;
    if (abs(minLeftDistance - minRightDistance) > 5) {
      //If min distances differs more than 5cm choose the safest direction
      //i.e. the direction corresponding to the max value
      uTurnCommand = minLeftDistance > minRightDistance ? U_TURN_LEFT : U_TURN_RIGHT;
      SerialPrint("Choosing direction using min distance: ");
      SerialPrintln(minLeftDistance > minRightDistance ? "LEFT": "RIGHT");
    } else if (abs(maxLeftDistance - maxRightDistance) > 5) {
      //If min distance are close each other and max distance differs more than 5cm
      //chose the direction corresponding to the max value
      uTurnCommand = maxLeftDistance > maxRightDistance ? U_TURN_LEFT : U_TURN_RIGHT;
      SerialPrint("Choosing direction using max distance: ");
      SerialPrintln(maxLeftDistance > maxRightDistance ? "LEFT": "RIGHT");
    } else {
      SerialPrintln("Coosing random direction");
    }
  }
  //Move motors to the desired direction
  motorsController->execute(uTurnCommand, delta );
  //Back to normal drive
  state = NormalDrive;
}

void AutoDriveController::onTickLookSide(bool isRight) {
  timer.interval(100);
  SerialPrint(" ServoState (");
  SerialPrint(servoState);
  SerialPrint("): ");
  if (servoState == Forward) {
    SerialPrint("Servo Look");
    SerialPrintln(isRight ? "Right" : "Left");
    //We just entered into this state: start moving the servo in order to check side
    servoController->setMode(isRight ? LOOK_RIGHT : LOOK_LEFT);
    servoState = Looking;
    if (isRight) {
      //Register the servo target reached callback
      servoController->setOnTargetReachedCallback([](ServoTargetEnum target) {
        //The lambda function is required for the registration
        AutoDriveController* pThis = reinterpret_cast<AutoDriveController*>(target);
        pThis->myServoTargetReceivedCallback(target); 
      });
    }
    //Start checking distances
    distanceSensor->startDistanceAnalysis();
  } else if (servoState == Looked) {
    //The servo reached the desired position. Return looking formard while still measuring distance
    SerialPrintln("Servo LookForward");
    servoController->setMode(LOOK_FORWARD);
    servoState = Resuming;
  } else if (servoState == Resumed) {
    SerialPrint("Servo Looked ");
    SerialPrintln(isRight ? "Right" : "Left");
    //The servo reached the FORWARD Position
    servoState = isRight? Forward : MotorHandled;
    if (!isRight) {
      //Unregister the callback
      servoController->setOnTargetReachedCallback(nullptr);
    }
    //Go to the next state
    state = isRight? LookLeft : Backward;
    //Stop Distance Analysis
    distanceSensor->stopDistanceAnalysis();
    //Calculate min/max distances
    float minDistance = distanceSensor->getMinDistance();
    float maxDistance = distanceSensor->getMaxDistance();
    SerialPrint("Calculated ");
    SerialPrint(isRight ? "Right" : "Left");
    SerialPrint(" Distance min: ");
    SerialPrint(minDistance);
    SerialPrint(" max: ");
    SerialPrintln(maxDistance);
    if (isRight) {
      minRightDistance = minDistance;
      maxRightDistance = maxDistance;
    } else {
      minLeftDistance = minDistance;
      maxLeftDistance = maxDistance;
    }
    //Stop Distance Analysis

  } else {
    SerialPrintln("Waiting");
  }
}

void AutoDriveController::onTick() {
  timer.stop();
  SerialPrint("OnTick State ");
  SerialPrintln(enumToString(state));
  switch (state) {
    case NormalDrive:
      onTickNormalDrive();
      break;
    case Stopped:
      //This state is fired when robot is in a dangerous situation
      //Don't move for 1 second
      timer.interval(1000);
      motorsController->execute(STOP);
      state = dangerRecoveryMode == SERVO_BASED_RECOVERY ? LookRight : Backward;
      //Setting ServoState to Forward triggers the servo to look for best direction to turn
      servoState = Forward;
      break;
    case Backward:
      //Move backward for 1 second (200ms if ServoBased Recovery)
      timer.interval(dangerRecoveryMode == SERVO_BASED_RECOVERY ? 200 : 1000);
      motorsController->execute(MOVE_BACKWARD);
      state = UTurn;      
      break;
    case UTurn:
      onTickUTurn();
      break;
    case LookRight:
      onTickLookSide(true);
      break;
    case LookLeft:
      onTickLookSide(false);
      break;
  }
  timer.start();
}

void AutoDriveController::onServoTargetReached(ServoTargetEnum target) {
  //This callback is executed when the servo reached the target
  SerialPrint("Servo AutoDrive callback Target (");
  SerialPrint(target);
  SerialPrint(") State (");
  SerialPrint(enumToString(state));
  SerialPrint(") ServoState (");
  SerialPrint(servoState);
  SerialPrint(") ");
  if (state == LookLeft || state == LookRight) {
    switch (target) {
      case LOOK_RIGHT:
      case LOOK_LEFT:
        SerialPrintln("IsTargetReached");
        servoState = Looked;
        break;
      case LOOK_FORWARD:
        SerialPrintln("IsForwardReached");
        servoState = Resumed;
        break;
      default:
        SerialPrintln("Invalid Target");
    }
  } else {
    //This should never happen (the callback shoult be registered only in the proper states)
    SerialPrintln("Invalid State");
  }
}

void AutoDriveController::start(DangerRecoveryModeEnum mode) {
    randomSeed(micros());
    //Set the recovery mode
    dangerRecoveryMode = mode;
    isEnabled = true;
    //Start the timer
    state = NormalDrive;
    //Execute the tick
    onTick();
}

void AutoDriveController::stop() {
    isEnabled = false;
    timer.stop();
}

bool AutoDriveController::getIsEnabled() {
    return isEnabled;
}

void AutoDriveController::update() {
  if (isEnabled) {
    timer.update();
  }
}

void AutoDriveController::setDangerMode() {
  state = Stopped;
  onTick();
}

void AutoDriveController::myServoTargetReceivedCallback(ServoTargetEnum target) {
  if (instance) {
    instance->onServoTargetReached(target);  
  }
}
