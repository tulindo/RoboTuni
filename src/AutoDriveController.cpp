#include "AutoDriveController.h"

//Initialize static variables (otherwise linker will fail)
AutoDriveController* AutoDriveController::instance = nullptr;

AutoDriveController::AutoDriveController(
    MotorsController* motorsController, 
    ServoController* servoController) :
  motorsController(motorsController),
  servoController(servoController),
  timer([this]() { onTick(); }, 1000, 0, MILLIS) {
    instance = this;
    dangerRecoveryMode = STANDARD_RECOVERY,
    isEnabled = false;
    commands[0] = { MOVE_FORWARD, 30, 2000, 4000 };
    commands[1] = { TURN_LEFT, 15, 1000, 2000 };
    commands[2] = { TURN_RIGHT, 15, 1000, 2000 };
    commands[3] = { MOVE_FORWARD_LEFT, 10, 1000, 2000 };
    commands[4] = { MOVE_FORWARD_RIGHT, 10, 1000, 2000 };
    commands[5] = { ROTATE_LEFT, 10, 1000, 2500 };
    commands[6] = { ROTATE_RIGHT, 10, 1000, 2500 };
}

void AutoDriveController::onTick() {
  timer.stop();
  int target;
  int current;
  short delta;
  SerialPrint("OnTick State (");
  SerialPrint(state);
  SerialPrint(") ");
  switch (state) {
    case NormalDrive:
      SerialPrintln("NormalDrive ");
      servoState = MotorHandled;
      //Get a target from 1 to 100
      target = random(1, 101);
      current = 0;
      for (int i=0; i<7; i++) {
        //Find first command based on random
        CommandData command = commands[i];
        current += command.percentage;
        if (current >= target) {
          //Command Found: execute it
          motorsController->execute(command.command);
          //Adjust timer interval based on configuration
          timer.interval(random(command.minDurationMillis, command.maxDurationMillis));
          break;
        } 
      }
      break;
    case Stopped:
      SerialPrintln("Stopped");
      //This state is fired when robot is in a dangerous situation
      //Don't move for 1 second
      timer.interval(1000);
      motorsController->execute(STOP);
      state = dangerRecoveryMode == SERVO_BASED_RECOVERY && servoState == Forward ? LookRight : Backward;
      break;
    case Backward:
      SerialPrintln("Backward");
      //Then move backward for 1 second (500ms if ServoBased Recovery)
      timer.interval(dangerRecoveryMode == SERVO_BASED_RECOVERY ? 500 : 1000);
      motorsController->execute(MOVE_BACKWARD);
      //In servo based recovery stops again to check for the best direction
      state = dangerRecoveryMode == SERVO_BASED_RECOVERY ? Stopped : UTurn;      
      servoState = Forward;
      break;
    case UTurn:
      SerialPrintln("U-Turn");
      //Finally do a U Turn 
      timer.interval(5000);
      delta = 0;
      if (dangerRecoveryMode == RANDOM_ROTATION_RECOVERY) {
        delta = random(-45, 45);
      }
      motorsController->execute(random(2) == 0 ? U_TURN_LEFT : U_TURN_RIGHT, delta );
      //Back to normal drive
      state = NormalDrive;
      break;
    case LookRight:
      SerialPrint("LookRight ServoState (");
      SerialPrint(servoState);
      SerialPrint("): ");
      timer.interval(100);
      if (servoState == Forward) {
        //Stop the motors
        SerialPrintln("Servo LookRight");
        motorsController->execute(STOP);
        //We just entered into this state: start moving the servo in order to check right
        servoController->setMode(LOOK_RIGHT);
        servoState = Looking;
        //Register the callback
        servoController->setOnTargetReachedCallback([](ServoTargetEnum target) {
          //The lambda function is required for the registration
          AutoDriveController* pThis = reinterpret_cast<AutoDriveController*>(target);
          pThis->myServoTargetReceivedCallback(target); 
        });
      } else if (servoState == Looked) {
        //The servo reached the LOOK_RIGHT position
        SerialPrintln("Servo LookForward");
        servoController->setMode(LOOK_FORWARD);
        servoState = Resuming;
      } else if (servoState == Resumed) {
        SerialPrintln("Servo Looked Right");
        //The servo reached the FORWARD Position
        servoState = Forward;
        //Go to the next state
        state = LookLeft;
      } else {
        SerialPrintln("Waiting");
      }
      break;
    case LookLeft:
      SerialPrint("LookLeft ServoState (");
      SerialPrint(servoState);
      SerialPrint("): ");
      timer.interval(100);
      if (servoState == Forward) {
        SerialPrintln("Servo LookLeft");
        //We just entered into this state: start moving the servo in order to check right
        servoController->setMode(LOOK_LEFT);
        servoState = Looking;
      } else if (servoState == Looked) {
        SerialPrintln("Servo LookForward");
        //The servo reached the LOOK_LEFT position
        servoController->setMode(LOOK_FORWARD);
        servoState = Resuming;
      } else if (servoState == Resumed) {
        //The servo reached the FORWARD Position
        SerialPrintln("Servo Looked Left");
        servoState = MotorHandled;
        //Unregister the callback
        servoController->setOnTargetReachedCallback(nullptr);
        //We finished looking for the best direction, so go to the next state
        state = UTurn;
      } else {
        SerialPrintln("Waiting");
      }
      break;
  }
  timer.start();
}

void AutoDriveController::onServoTargetReached(ServoTargetEnum target) {
  //This callback is executed when the servo reached the target
  SerialPrint("SERVO AutoDrive callback Target (");
  SerialPrint(target);
  SerialPrint(") State (");
  SerialPrint(state);
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
