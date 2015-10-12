// setup_living_room.c -------------------------------------------
#include "example.h"
int const ACTION_COUNT = 8;
int const BUTTON1_ACTION_ID = 5;
int const BUTTON2_ACTION_ID = 1;
const long displayReset = 1000;

#define NECBit 32
#define NECRep 1
#define NECDelay 40
#define SONYBit 15
#define SONYRep 3
#define SONYDelay 40
#define HeaderTV "TV"
#define HeaderPower "Power"
IRAction irActions[ACTION_COUNT] = {
	{"TV Source", HeaderTV, 1, {{NEC, 0x209DB847, NECBit, NECRep, NECDelay}}},
	{"TV Menu", HeaderTV, 1, {{NEC, 0x209DD827, NECBit, NECRep, NECDelay}}},
	{"TV UP", HeaderTV, 1, {{NEC, 0x209D08F7, NECBit, NECRep, NECDelay}}},
	{"TV Down", HeaderTV, 1, {{NEC, 0x209D8877, NECBit, NECRep, NECDelay}}},
	{"TV Enter", HeaderTV, 1, {{NEC, 0x209D38C7, NECBit, NECRep, NECDelay}}},

	{"Power", HeaderPower, 2, {
		{SONY, 0x540c, SONYBit, SONYRep, SONYDelay, 75}, // Delay of 75ms between commands
		{NEC, 0x209D00FF, NECBit, NECRep, NECDelay}}
	},
	{"TV Power", HeaderPower, 1, {{NEC, 0x209D00FF, NECBit, NECRep, NECDelay}}},
	{"RCV Power", HeaderPower, 1, {{SONY, 0x540c, SONYBit, SONYRep, SONYDelay}}},
};
