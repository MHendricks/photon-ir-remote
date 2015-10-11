// example.h ---------------------------------------------

#ifndef EXAMPLE_H
#define EXAMPLE_H

#include "IRremote.h"

// Maximum number of commands
int const DATA_COUNT = 2;
extern int const BUTTON_COUNT;
extern int const BUTTON_ACTION_ID;

typedef struct {
  int sysType; // Send data type
  unsigned long data; // Data to send
  int nbits; // Number of bits to send
  int repeat; // How many times to repeat the message
  int repeatDelay; // Delay between repeats
  int delay; // Delay before sending next command
} IRCommand;

typedef struct {
  char *displayName;
  int commandCount;
  IRCommand commands[DATA_COUNT];
} IRAction;

extern IRAction irActions[];

#endif // EXAMPLE_H
