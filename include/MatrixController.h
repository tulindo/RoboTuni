#ifndef _MATRIX_CONTROLLER_H
#define _MATRIX_CONTROLLER_H

#include <Arduino.h>

#include "RobotCommands.h"
#include <Arduino_LED_Matrix.h>

#include <Ticker.h>

#include "serial.h"

enum ImageEnum : byte {
  IMG_LOGO_BLE = 200,
  IMG_LOGO_BLE_DOT = 201,
  IMG_ERROR = 202,
  IMG_BATTERY_0 = 203,
  IMG_BATTERY_30 = 204,
  IMG_BATTERY_60 = 205,
  IMG_BATTERY_100 = 206,
  IMG_CLEAR = 207
};

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

  void showImage(ImageEnum image);
};

#endif