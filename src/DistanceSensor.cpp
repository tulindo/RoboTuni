#include "DistanceSensor.h"

#define MAX_PULSE_TIME 999999999

DistanceSensor::DistanceSensor(int trigPin, int echoPin, int maxDistance) : 
  trigPin(trigPin), 
  echoPin(echoPin),
  maxDistance(maxDistance),
  sonar(trigPin, echoPin, maxDistance) {
    isSinglePin = trigPin == echoPin;
}

float DistanceSensor::convertTimeToDistance(unsigned int time) {
  return pulseTime == MAX_PULSE_TIME ? (float)maxDistance : time * 0.0343 / 2;
}

float DistanceSensor::getDistance() {
  //Convert pulseTime to cm.
  //SerialPrint(F("Measured distance in cm: "));
  float distance = convertTimeToDistance(pulseTime);
  // SerialPrint(F("Distance: "));
  // SerialPrintln(distance);
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
    if (isSinglePin) {
      pinMode(trigPin, OUTPUT);
    }
    trigStartTime = micros();
  }

  //Set trigger high for 10microseconds in order to start measuring
  if (stato == TRIGGER_HIGH) {
    digitalWrite(trigPin, HIGH);
    if (micros() - trigStartTime >= 10) {
      //10 microseconds passed. change state and deactivate trigger
      digitalWrite(trigPin, LOW);
      stato = WAITING_FOR_ECHO;
      if (isSinglePin) {
        pinMode(echoPin, OUTPUT);
      }
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
  if (pulseTime == 0) {
    //NewPing library returns 0 when the measured distance is above the configured max distance
    //so, if this time is 0 return an invalid value.
    pulseTime = MAX_PULSE_TIME;
  }
  if (isAnalyzingDistance) {
    SerialPrint(F("Detected pulseTime: "));
    SerialPrint(pulseTime);
    SerialPrint(F(" Current Min: "));
    SerialPrint(currentMinPulseTime);
    SerialPrint(F(" Current Max: "));
    SerialPrintln(currentMaxPulseTime);
  }
  if (isAnalyzingDistance && pulseTime > currentMaxPulseTime) {
    currentMaxPulseTime = pulseTime;
  }
  if (isAnalyzingDistance && pulseTime < currentMinPulseTime) {
    currentMinPulseTime = pulseTime;
  }
}

void DistanceSensor::startDistanceAnalysis() {
  SerialPrintln(F("Starting Distance Analysis"));
  currentMinPulseTime = currentMaxPulseTime = pulseTime;
  isAnalyzingDistance = true; 
} 

void DistanceSensor::stopDistanceAnalysis() {
  SerialPrintln(F("Stopping Distance Analysis"));
  isAnalyzingDistance = false;
}

float DistanceSensor::getMinDistance() {
  return convertTimeToDistance(currentMinPulseTime);
}

float DistanceSensor::getMaxDistance() {
  return convertTimeToDistance(currentMaxPulseTime);
}