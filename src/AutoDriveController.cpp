#include "AutoDriveController.h"

AutoDriveController::AutoDriveController(
    MotorsController* motorsController, 
    ServoController* servoController,
    DangerRecoveryModeEnum dangerRecoveryMode) :
  motorsController(motorsController),
  servoController(servoController),
  dangerRecoveryMode(dangerRecoveryMode),
  timer([this]() { onTick(); }, 1000, 0, MILLIS) {
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
  switch (state) {
    case NormalDrive:
      SerialPrint("NormalDrive ");
      //Get a target from 1 to 100
      target = random(1, 101);
      current = 0;
      for (int i=0; i<7; i++) {
        //Find first command based on random
        CommandData command = commands[i];
        current += command.percentage;
        if (current >= target) {
          //Command Found: execute it
          SerialPrintln(command.command);
          motorsController->execute(command.command);
          //Adjust timer interval based on configuration
          timer.interval(random(command.minDurationMillis, command.maxDurationMillis));
          break;
        } 
      }
      break;
    case Stopped:
      SerialPrintln(" Stopped");
      //This state is fired when robot is in a dangerous situation
      //Don't move for 1 second
      timer.interval(1000);
      motorsController->execute(STOP);
      state = Backward;
      break;
    case Backward:
      SerialPrintln(" Backward");
      //Then move backward for 1 second
      timer.interval(1000);
      motorsController->execute(MOVE_BACKWARD);
      state = UTurn;
      break;
    case UTurn:
      SerialPrintln("U-Turn");
      //Finally do a U Turn 
      timer.interval(5000);
      motorsController->execute(random(2) == 0 ? U_TURN_LEFT : U_TURN_RIGHT );
      //Back to normal drive
      state = NormalDrive;
  }
  timer.start();
}


void AutoDriveController::start() {
    randomSeed(micros());
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

void AutoDriveController::setDangetMode() {
  state = Stopped;
  onTick();
}
