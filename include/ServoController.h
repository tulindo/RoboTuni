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

    //Internal Timer
    Ticker timer;
    void onTick();
    
    //The servo driver
    Servo servo;

    //The desired target
    ServoTargetEnum servoTarget;
    //True if oscillation is requested after reaching target
    bool isOscillation;
    //Current Servo position
    byte servoPosition;
    //The desired target position
    byte targetPosition;

    bool isIncreasingPosition;
    bool isTargetReached;
    
    //Servo Steps to move when reaching target
    int targetReachingSteps;

    //Pointer for the callback
    void (*onTargetReachedCallback)(ServoTargetEnum); 
    static ServoController* instance;

  public:

    //Constructor
    explicit ServoController(int servoPin, int timerDelay);

    //Initialize servo
    void begin();

    //Set Servo mode
    void setMode(ServoTargetEnum target, bool oscillation = false);

    //Update timer
    void update();

    //Set the callback that will be called when the servo reached the desired target
    void setOnTargetReachedCallback(void (*callback)(ServoTargetEnum));
};

#endif