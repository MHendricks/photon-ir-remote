// example.h ---------------------------------------------

#ifndef EXAMPLE_H
#define EXAMPLE_H

#include "IRremote.h"

extern int const BUTTON_COUNT;
int const DATA_COUNT = 2;

typedef struct {
  int sysType;
  unsigned long data;
  int nbits;
  int delay;
} IRData;

typedef struct {
  char *displayName;
  //char *irId; // I'm going to drop this for irData index
  IRData commands[DATA_COUNT];
} IRAction;

extern IRAction irActions[];

#endif // EXAMPLE_H
