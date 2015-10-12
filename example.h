// example.h ---------------------------------------------

#ifndef EXAMPLE_H
#define EXAMPLE_H

#include "IRremote.h"

// Maximum number of IRCommands
int const COMMAND_COUNT = 2;
// Total number of IRActions served by this device
extern int const ACTION_COUNT;
// The Photon digital pins used as buttons
extern int const BUTTON1_ACTION_ID;
extern int const BUTTON2_ACTION_ID;
// Clear the OLED display after this many millis
extern const long displayReset;

typedef struct {
  int sysType; // Send data type
  unsigned long data; // Data to send
  int nbits; // Number of bits to send
  int repeat; // How many times to repeat the message
  int repeatDelay; // Delay between repeats
  int delay; // Delay before sending next command
} IRCommand;

typedef struct {
  char *displayName; // Name shown on the button
  /* All buttons with the same header defined sequencially will be grouped
     under the same header in the Web Page. */
  char *header;
  // The number of IRCommands in this action
  int commandCount;
  // Array of IRCommands to send when this action is called.
  IRCommand commands[COMMAND_COUNT];
} IRAction;

extern IRAction irActions[];

#endif // EXAMPLE_H
