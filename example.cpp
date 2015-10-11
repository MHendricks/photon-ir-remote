// setup_living_room.c -------------------------------------------
#include "example.h"
int const BUTTON_COUNT = 6;
IRAction irActions[BUTTON_COUNT] = {
	{"TV Source", {{NEC, 0x209DB847, 32}}},
	{"TV Menu", {{NEC, 0x209DD827, 32}}},
	{"TV UP", {{NEC, 0x209D08F7, 32}}},
	{"TV Down", {{NEC, 0x209D8877, 32}}},
	{"TV Enter", {{NEC, 0x209D38C7, 32}}},

	{"Power", {
		{SONY,0x540c,15, 75}, // Delay of 75ms
		{NEC,0x209D00FF,32}}
	}
};
