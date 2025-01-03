#include "InfraredSensors.h"

InfraredSensors::InfraredSensors(int pinLeft, int pinRight) :
  pinLeft(pinLeft), pinRight(pinRight) {
    pinMode(pinLeft, INPUT);
    pinMode(pinRight, INPUT);
}

bool InfraredSensors::obstacleDetected() {
  return digitalRead(pinLeft) == LOW || digitalRead(pinRight) == LOW;
}
