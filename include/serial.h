#ifndef _SERIAL_DEFINES_H
#define _SERIAL_DEFINES_H

#define VERBOSE 1
#if VERBOSE 
#define SerialBegin(...)    Serial.begin(__VA_ARGS__)
#define SerialPrint(...)    Serial.print(__VA_ARGS__)
#define SerialWrite(...)    Serial.write(__VA_ARGS__)
#define SerialPrintln(...)  Serial.println(__VA_ARGS__)
#else
#define SerialBegin(...)
#define SerialPrint(...)
#define SerialWrite(...)
#define SerialPrintln(...)
#endif

#endif