#ifndef QUEUE_H
#define QUEUE_H

#include <Arduino.h>
#include "RobotEnums.h"
#include "serial.h"

struct QueueData {
    RobotCommandEnum command;      //The command that caused this entry to be pushed on the queue
    short leftMotorSpeed;          //The speed for the left motor
    short rightMotorSpeed;         //The speed for the right motor

    // Costruttore
    QueueData(RobotCommandEnum cmd = RobotCommandEnum::UNKNOWN_COMMAND, short ls = 0, short rs=0) 
        : command(cmd), leftMotorSpeed(ls), rightMotorSpeed(rs) {
    }
};

class Queue {
  private:
    static const int MAX_SIZE = 10;  //Maximum queue size
    struct QueueDataInternal {
      QueueData value;          
      unsigned short occurrence;     //Number of times the speeds has to be retrieved
    } queue[MAX_SIZE];               //Circular buffer that stored the entries
    int front;                       //Index of the first element of the queue
    int rear;                        //Index of the last element of the queue
    int itemCount;                   //Number of elements in the queue

  public:
    Queue();

    //Add and element (or n copies of the element itself) to the queue
    bool enqueue(QueueData value, unsigned short occurrence = 1);

    //Gets an element from the queue
    QueueData dequeue();

    //Gets the first element from the queue without removing it
    QueueData peek() const;

    //Check if the queue is empty
    bool isEmpty() const;

    //Check if the queue is full
    bool isFull() const;

    //Get the numbers of elements in the quueue
    int size() const;

    //Empties the queue
    void clear();
};

#endif
