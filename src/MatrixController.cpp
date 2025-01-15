#include "MatrixController.h"

const MatrixController::ImageData MatrixController::images[] = {
  //Motor command images
  { MOVE_FORWARD,        { 0x0C018030, 0x07FE7FE3, 0x001800C0 }},
  { MOVE_BACKWARD,       { 0x03001800, 0xC7FE7FE0, 0x0C018030 }},
  { MOVE_FORWARD_RIGHT,  { 0x1803C07E, 0x0DB09901, 0x801FF1FF }},
  { MOVE_BACKWARD_RIGHT, { 0x01803C07, 0xE0DB0990, 0x18FF8FF8 }},
  { MOVE_FORWARD_LEFT,   { 0x1FF1FF18, 0x0990DB07, 0xE03C0180 }},
  { MOVE_BACKWARD_LEFT,  { 0xFF8FF801, 0x80990DB0, 0x7E03C018 }},
  { TURN_RIGHT,          { 0x0600F01F, 0x836C2640, 0x60060060 }},
  { TURN_LEFT,           { 0x06006006, 0X026436C1, 0xF80F0060 }},
  { ROTATE_LEFT,         { 0x3FC60280, 0x18218626, 0x7E060020 }},
  { ROTATE_RIGHT,        { 0x0400603F, 0x64618418, 0x014023fC }},
  { U_TURN_LEFT,         { 0x01800C3F, 0XE40C4184, 0x004003FE }},
  { U_TURN_RIGHT,        { 0x3FE40040, 0x014840C3, 0xFE0040C3 }},
  { AUTO_DRIVE,          { 0x18224452, 0x88918915, 0x28244182 }},
  { STOP,                { 0x0F010820, 0x42642642, 0x041080F0 }},
  { EMERGENCY_STOP,      { 0x3FC40280, 0x1BEDBED8, 0x014023FC }},
  { UNKNOWN_COMMAND,     { 0x38044082, 0x081D8808, 0x40440380 }},
  //Other images
  { IMG_LOGO_BLE,        { 0x00020450, 0xA891FFF0, 0x60090108 }},
  { IMG_LOGO_BLE_DOT,    { 0x00120450, 0xA891FFF0, 0x60090108 }},
  { IMG_ERROR,           { 0x20410809, 0x00600600, 0x90108204 }},
  { IMG_BATTERY_0,       { 0x0007FF40, 0x1C01C014, 0x017FF000 }},
  { IMG_BATTERY_30,      { 0x0007FF40, 0xFC0FC0F4, 0x0F7FF000 }},
  { IMG_BATTERY_60,      { 0x0007FF47, 0xFC7FC7f4, 0x7F7FF000 }},
  { IMG_BATTERY_100,     { 0x0007FF7F, 0xFFFFFFF7, 0xFF7FF000 }},
  { IMG_CLEAR,           { 0x00000000, 0x00000000, 0x00000000 }}
};

MatrixController::MatrixController() : 
  ArduinoLEDMatrix(), 
  timer([this]() { onTick(); }, 1000, 0, MILLIS) {
}

void MatrixController::onTick() {
  showImage(IMG_CLEAR);
  timer.stop();
}

//Without this apparently useless method matrix doesn't display images
void MatrixController::begin() {
  ArduinoLEDMatrix::begin();
}

void MatrixController::update() {
  timer.update();
}

const uint32_t* MatrixController::getImageByKey(byte key) const {
  for (const auto& img : images) {
    if (img.key == key) {
      return img.image;
    }
  }
  return nullptr;
}


void MatrixController::showCommand(RobotCommandEnum command) {
  const uint32_t* frame = getImageByKey(command);
  if (frame != nullptr) {
    loadFrame(frame);
    //start the timer that will delete the image after 1 second
    timer.start();
  } else {
    SerialPrint("Command image ");
    SerialPrint(enumToString(command));
    SerialPrintln(" not found!");
  }
}

void MatrixController::showImage(MatrixImageEnum image) {
  const uint32_t* frame = getImageByKey(image);
  if (frame != nullptr) {
    loadFrame(frame);
  } else {
    SerialPrint("Image ");
    SerialPrint(enumToString(image));
    SerialPrintln(" not found!");
  }
}
