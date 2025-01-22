#include "Queue.h"

//Constructor
Queue::Queue() : front(0), rear(-1), itemCount(0) {}

//Add an item to the queue
bool Queue::enqueue(QueueData value, unsigned short occurrence) {
  // SerialPrint("Enqueue Left: ");
  // SerialPrint(value.leftMotorSpeed);
  // SerialPrint(" Right: ");
  // SerialPrint(value.rightMotorSpeed);
  // SerialPrint(" Occurrence: ");
  // SerialPrintln(occurrence);
  if (isFull()) {
    return false;  // Coda piena
  }
  rear = (rear + 1) % MAX_SIZE;
  queue[rear].value = value;
  queue[rear].occurrence = occurrence;
  itemCount++;
  return true;
}

//Get an item from the queue
QueueData Queue::dequeue() {
  if (isEmpty()) {
    //Return an empty element... should never happen
    return QueueData();
  }
  queue[front].occurrence--;
  QueueData value = queue[front].value;
  if (queue[front].occurrence == 0) {
    //go to next element
    front = (front + 1) % MAX_SIZE;
    itemCount--;
  }
  return value;
}

//Peek an item from the queue (leaving it in the queue)
QueueData Queue::peek() const {
  if (isEmpty()) {
    return QueueData(); 
  }
  return queue[front].value;
}

bool Queue::isEmpty() const {
  return itemCount == 0;
}

bool Queue::isFull() const {
  return itemCount == MAX_SIZE;
}

int Queue::size() const {
  return itemCount;
}

void Queue::clear() {
  front = 0;
  rear = -1;
  itemCount = 0;
}