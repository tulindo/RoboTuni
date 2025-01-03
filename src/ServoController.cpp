#include "ServoController.h"

ServoController::ServoController(int servoPin, int timerDelay) : 
  servoPin(servoPin),
  servo(),
  timer([this]() { onTick(); }, timerDelay, 0, MILLIS) {
    targetReachingSteps = TARGET_REACHING_STEPS;
}

void ServoController::begin() {
  servoPosition = DEFAULT_SERVO_POSITION;
  targetPosition = DEFAULT_SERVO_POSITION;
  isIncreasingPosition = true;
  servoTarget = LOOK_FORWARD;
  isOscillation = false;
  servo.attach(servoPin);
  servo.write(servoPosition);
  timer.start();
}

void ServoController::onTick() { 
  int step = 0;
  if (!isTargetReached) {
    //Reach the TargetPosition first (stepping targetReachingSteps at a time)
    step = isIncreasingPosition ? 
      min(targetPosition - servoPosition, targetReachingSteps) : 
        max(targetPosition - servoPosition, -targetReachingSteps);
    if (step == 0) {
      isTargetReached = true;
      //TODO: Notify Callback
    }
  } else if (isOscillation) {
    //Implement Oscillation around the targetPosition
    if (isIncreasingPosition) {
      isIncreasingPosition = servoPosition != targetPosition + OSCILLATING_RANGE;
    } else {
      isIncreasingPosition = servoPosition == targetPosition - OSCILLATING_RANGE;
    }
    step = isIncreasingPosition ? 1 : -1;
  }
  if (step != 0) {
    // SerialPrint("Servo TargetPosition : ");
    // SerialPrint(targetPosition);
    // SerialPrint(" Position: ");
    servoPosition += step;
    // SerialPrintln(servoPosition);
    servo.write(servoPosition);
  }
}

void ServoController::update() {
  timer.update();
}

void ServoController::setMode(ServoTargetEnum target, bool oscillation) {
  // SerialPrint("Setting Mode ");
  servoTarget = target;
  //Set target position based on desired target
  switch (servoTarget) {
    case LOOK_FORWARD:
      targetPosition = DEFAULT_SERVO_POSITION;
      break;
    case LOOK_FORWARD_LEFT:
      targetPosition = DEFAULT_SERVO_POSITION + FORWARD_SIDE_OFFSET;
      break;
    case LOOK_FORWARD_RIGHT:
      targetPosition = DEFAULT_SERVO_POSITION - FORWARD_SIDE_OFFSET;
      break;
    case LOOK_LEFT:
      targetPosition = DEFAULT_SERVO_POSITION + LOOK_SIDE_OFFSET;
      break;
    case LOOK_RIGHT:
      targetPosition = DEFAULT_SERVO_POSITION - LOOK_SIDE_OFFSET;
      break;
  }
  //True if the servo position has to increase to reach the desired targer
  isIncreasingPosition = servoPosition < targetPosition;
  //First of all the target has to be reached
  isTargetReached = false;
  //Tell if, after reaching the target, there will be some oscillation.
  isOscillation = oscillation;
  // SerialPrint(mode);
  // SerialPrint(" Target: ");
  // SerialPrint(targetPosition);
  // SerialPrint(" Oscillation: ");
  // SerialPrintln(oscillation);
}
