#ifndef _MATRIX_CONTROLLER_H
#define _MATRIX_CONTROLLER_H

#include <Arduino.h>

#include "RobotEnums.h"
#include <Arduino_LED_Matrix.h>

#include <Ticker.h>

#include "serial.h"

class MatrixController : public ArduinoLEDMatrix {
private:
  struct ImageData {
    byte key; 
    const uint32_t image[3];
  };

  static const ImageData images[];

  const uint32_t* getImageByKey(byte key) const;

  Ticker timer;
  void onTick();

public:
  explicit MatrixController();        

  void begin();
    
  void update();
  
  void showCommand(RobotCommandEnum command);

  void showImage(MatrixImageEnum image);
};

#endif