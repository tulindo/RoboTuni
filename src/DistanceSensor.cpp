#include "DistanceSensor.h"

DistanceSensor::DistanceSensor(int trigPin, int echoPin, int maxDistance) : 
  trigPin(trigPin), 
  echoPin(echoPin),
  maxDistance(maxDistance),
  sonar(trigPin, echoPin, maxDistance) {
}

float DistanceSensor::getDistance() {
  //Convert pulseTime to cm.
  //SerialPrint("Measured distance in cm: ");
  //NewPing library returns 0 when the measured distance is above the configured max distance
  //so, if the median is 0 return maxDistance otherwise convert the delay in cm.
  float distance = pulseTime == 0 ? (float)maxDistance : pulseTime * 0.0343 / 2;
  SerialPrint("Distance: ");
  SerialPrintln(distance);
  return distance;
}

//Non blocking distance state machine
void DistanceSensor::distanceStateMachine(unsigned long interval) {
  unsigned long currentMillis = millis();

  //Check if tick time passed
  if (currentMillis - previousMillis >= interval && stato == TRIGGER) {
    previousMillis = currentMillis;

    //Change state and register trigger start time.
    stato = TRIGGER_HIGH;
    trigStartTime = micros();
  }

  //Set trigger high for 10microseconds in order to start measuring
  if (stato == TRIGGER_HIGH) {
    digitalWrite(trigPin, HIGH);
    if (micros() - trigStartTime >= 10) {
      //10 microseconda passed. change state and deactivate trigger
      digitalWrite(trigPin, LOW);
      stato = WAITING_FOR_ECHO;
      echoStarted = false; // Echo has not yet been sent.
    }
  }

  //Handle Echo state
  if (stato == WAITING_FOR_ECHO) {
    if (digitalRead(echoPin) == HIGH && !echoStarted) {
      //Echo is high and we are not yet measurning time. Do it.
      startEchoTime = micros();
      echoStarted = true;
    }

    if (digitalRead(echoPin) == LOW && echoStarted) {
      //Echo signal returner low. Record time and change to final state in order to calculate distance
      endEchoTime = micros();
      stato = CALCULATE_DISTANCE;
    }
  }

  //Calculate the distance
  if (stato == CALCULATE_DISTANCE) {
    //Calculate the time the echo pin has been high
    pulseTime = endEchoTime - startEchoTime;

    //Wait for the next measure
    stato = TRIGGER;
  }
}

void DistanceSensor::distanceBlocking() {
  //Calculate pulseTime using the NewPing Library
  pulseTime = sonar.ping();
}