#ifndef _SERIAL_DEFINES_H
#define _SERIAL_DEFINES_H

#define VERBOSE 1

#if VERBOSE 
#define SerialPrint(...)    if (isDebug) Serial.print(__VA_ARGS__)
#define SerialPrintln(...)  if (isDebug) Serial.println(__VA_ARGS__)

#else
#define SerialPrint(...)
#define SerialPrintln(...)
#endif

#endif