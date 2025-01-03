#include "Buzzer.h"

Buzzer::Buzzer(int pinBuzz) :
  pinBuzz(pinBuzz), 
  timer([this]() { onTick(); }, 1000, 0, MILLIS) {
    pinMode(pinBuzz, INPUT);
    noTone(pinBuzz);
}

void Buzzer::onTick() {
  noTone(pinBuzz);
  timer.stop();
}

void Buzzer::update() {
  timer.update();
}

void Buzzer::buzz(int frequency) {
  tone(pinBuzz, frequency);
  //start the timer that will stop the buzz
  timer.start();
}
