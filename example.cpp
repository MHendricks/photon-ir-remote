// setup_living_room.c -------------------------------------------
#include "example.h"
int const BUTTON_COUNT = 6;
IRAction idData[BUTTON_COUNT] = {
	{"tvSource","TV Source", {{NEC, 0x209DB847, 32}}},
	{"tvMenu","TV Menu", {{NEC, 0x209DD827, 32}}},
	{"tvUp","TV UP", {{NEC, 0x209D08F7, 32}}},
	{"tvDown","TV Down", {{NEC, 0x209D8877, 32}}},
	{"tvMenu","TV Enter", {{NEC, 0x209D38C7, 32}}},

	{"power", "Power", {
		{SONY,0x540c,15, 75}, // Delay of 75ms
		{NEC,0x209D00FF,32}}
	}
};
