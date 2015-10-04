// example.h ---------------------------------------------

#ifndef EXAMPLE_H
#define EXAMPLE_H

#include "IRremote.h"

typedef struct {
  int sysType;
  unsigned long data;
  int nbits;
  int delay;
} IRData;

typedef struct {
  char *displayName;
  char *irId; // I'm going to drop this for irData index
  IRData commands[2];
} IRAction;

extern int const BUTTON_COUNT;
extern IRAction idData[];

#endif // EXAMPLE_H
