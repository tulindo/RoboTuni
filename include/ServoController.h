#ifndef _SERVO_CONTROLLER_H
#define _SERVO_CONTROLLER_H

#include <Arduino.h>
#include <Ticker.h>
#include <Servo.h>
#include "serial.h"

#define DEFAULT_SERVO_POSITION   82
#define OSCILLATING_RANGE        10
#define FORWARD_SIDE_OFFSET      20
#define LOOK_SIDE_OFFSET         60
#define TARGET_REACHING_STEPS     5

enum ServoTargetEnum : byte {
  LOOK_FORWARD,
  LOOK_FORWARD_LEFT,
  LOOK_FORWARD_RIGHT,
  LOOK_LEFT,
  LOOK_RIGHT
};

class ServoController {
  private:
    //Servo Pin
    const int servoPin; // Pin Servo

    Ticker timer;
    Servo servo;
    ServoTargetEnum servoTarget;
    bool isOscillation;
    byte servoPosition;
    byte targetPosition;
    bool isIncreasingPosition;
    bool isTargetReached;
    int targetReachingSteps;
    void onTick();

  public:

    //Constructor
    explicit ServoController(int servoPin, int timerDelay);

    //Initialize servo
    void begin();

    void setMode(ServoTargetEnum target, bool oscillation = false);

    //Update timer
    void update();
};

#endif