#ifndef _BUZZER_H
#define _BUZZER_H

#include <Arduino.h>

#include <Ticker.h>

#include "serial.h"

class Buzzer {
private:
  int pinBuzz;
  Ticker timer;
  void onTick();

public:
  explicit Buzzer(int pinBuzz);        

  void update();
  
  void buzz(int frequency);
};

#endif