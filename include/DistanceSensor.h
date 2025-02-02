#ifndef _DISTANCE_SENSOR_H
#define _DISTANCE_SENSOR_H

#include <Arduino.h>
#include <NewPing.h>
#include "serial.h"
#include <EEPromConfiguration.h>

class DistanceSensor {
  private:
    //Trigger and Echo pins
    const int trigPin;  // Pin TRIG
    const int echoPin;  // Pin ECHO

    //Maximum distance measured by the sensor
    const int maxDistance;

    unsigned int pulseTime;

    //Variables for the non blocking state machine calculation
    bool isSinglePin = false;
    unsigned long previousMillis = 0; 
    unsigned long trigStartTime = 0;  //Time when triggering the pulse Tempo in cui è stato attivato il pin TRIG
    //Possible states of the state machine
    enum State { TRIGGER, WAITING_FOR_ECHO, CALCULATE_DISTANCE, TRIGGER_HIGH };
    State stato = TRIGGER;
    unsigned long startEchoTime = 0;
    unsigned long endEchoTime = 0;
    bool echoStarted = false;
    bool isAnalyzingDistance = false;
    unsigned int currentMinPulseTime;
    unsigned int currentMaxPulseTime;

    //NewPing sonar (used by the blocking calculation)
    NewPing sonar;
    float inline convertTimeToDistance(unsigned int time);

    //Debug internal variable
    bool isDebug; 

  public:

    //Constructor
    explicit DistanceSensor(int trigPin, int echoPin, int maxDistance);

    //Inizialize the sensor
    void begin();

    //Get the calculated distance based on the last measures
    float getDistance();

    //Calculate distance using a non blocking state machine
    void distanceStateMachine(unsigned long intervallo);

    //Calculate distance using a blocking NewPing call 
    void distanceBlocking();

    //Methods for distance Analysis
    void startDistanceAnalysis();
    void stopDistanceAnalysis();

    float getMaxDistance();
    float getMinDistance();
};

#endif