// setup_living_room.c -------------------------------------------
#include "example.h"
// Number of available actions for this remote
int const BUTTON_COUNT = 6;
/* When the button is presssed, run this action. Actions are referenced by
 	 their index in irActions */
int const BUTTON_ACTION_ID = 1;
#define NECBit 32
#define NECRep 1
#define NECDelay 40
IRAction irActions[BUTTON_COUNT] = {
	{"TV Source", 1, {{NEC, 0x209DB847, NECBit, NECRep, NECDelay}}},
	{"TV Menu", 1, {{NEC, 0x209DD827, NECBit, NECRep, NECDelay}}},
	{"TV UP", 1, {{NEC, 0x209D08F7, NECBit, NECRep, NECDelay}}},
	{"TV Down", 1, {{NEC, 0x209D8877, NECBit, NECRep, NECDelay}}},
	{"TV Enter", 1, {{NEC, 0x209D38C7, NECBit, NECRep, NECDelay}}},

	{"Power", 2, {
		{SONY, 0x540c,15, 3, 40, 75}, // Delay of 75ms
		{NEC, 0x209D00FF, NECBit, NECRep, NECDelay}}
	}
};
