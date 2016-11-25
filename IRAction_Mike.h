/* Example of the config header used to define all of the actions the IR Blaster
	will show on its web page and the ir commands it sends when the buttons are
	pressed in the web page and on the device.
 */
#include "IRAction.h"
int const ACTION_COUNT = 20;
int const BUTTON1_ACTION_ID = 5;
int const BUTTON2_ACTION_ID = 1;
const long displayReset = 10000;

#define NECBit 32
#define NECRep 1
#define NECDelay 40
#define NECRptDelay 300
#define SONYBit 15
#define SONYRep 4
#define SONYDelay 30
#define SONYRptDelay 200
#define HeaderGen "General"
#define HeaderTV "TV"
#define HeaderRcv "Receiver"
#define HeaderPower "Power"

// Command definitions that are used multiple times.
IRCommand tvSource = {NEC, 0x209DB847, NECBit, NECRep, NECDelay, NECRptDelay};
IRCommand tvMenu = {NEC, 0x209DD827, NECBit, NECRep, NECDelay, NECRptDelay};
IRCommand tvUp = {NEC, 0x209D08F7, NECBit, NECRep, NECDelay, NECRptDelay};
IRCommand tvDown = {NEC, 0x209D8877, NECBit, NECRep, NECDelay, NECRptDelay};
IRCommand tvEnter = {NEC, 0x209D38C7, NECBit, NECRep, NECDelay, NECRptDelay};

IRCommand rcvInputMulti = {SONY, 0x270C, SONYBit, SONYRep, SONYDelay};
IRCommand rcvInputVideo2 = {SONY, 0x3C0C, SONYBit, SONYRep, SONYDelay};

IRAction irActions[ACTION_COUNT] = {
	{"volume up", "Volume Up", HeaderGen, 1, {{SONY, 0x240C, SONYBit, SONYRep, SONYDelay, SONYRptDelay}}},
	{"volume down", "Volume Dn", HeaderGen, 1, {{SONY, 0x640C, SONYBit, SONYRep, SONYDelay, SONYRptDelay}}},
	{"mute", "Mute", HeaderGen, 1, {{SONY, 0x140C, SONYBit, SONYRep, SONYDelay}}},
	{"ps3", "PS3", HeaderGen, 12, {
      tvMenu,
      tvUp,
      tvEnter,
      tvDown,
      tvEnter,
      tvDown,
      tvDown,
      tvEnter,
      tvMenu,
      tvMenu,
      tvMenu,
      rcvInputVideo2
    }
  },
	{"computer", "Computer", HeaderGen, 3, {
      tvSource,
      tvSource,
      rcvInputMulti
    }
  },

	{"source", "Source", HeaderTV, 1, {{NEC, 0x209DB847, NECBit, NECRep, NECDelay}}},
	{"menu", "Menu", HeaderTV, 1, {{NEC, 0x209DD827, NECBit, NECRep, NECDelay}}},
	{"enter", "Enter", HeaderTV, 1, {{NEC, 0x209D38C7, NECBit, NECRep, NECDelay}}},
	{"up", "UP", HeaderTV, 1, {{NEC, 0x209D08F7, NECBit, NECRep, NECDelay}}},
	{"down", "Down", HeaderTV, 1, {{NEC, 0x209D8877, NECBit, NECRep, NECDelay}}},


	{"video 1", "Video 1", HeaderRcv, 1, {{SONY, 0x220C, SONYBit, SONYRep, SONYDelay}}},
	{"video 2", "Video 2", HeaderRcv, 1, {rcvInputVideo2}},
	{"dvd", "DVD", HeaderRcv, 1, {{SONY, 0x5F0C, SONYBit, SONYRep, SONYDelay}}},
	{"tuner", "Tuner", HeaderRcv, 1, {{SONY, 0x420C, SONYBit, SONYRep, SONYDelay}}},
	{"multi channel", "Multi Ch", HeaderRcv, 1, {rcvInputMulti}},
	{"tape", "Tape", HeaderRcv, 1, {{SONY, 0x4B0C, SONYBit, SONYRep, SONYDelay}}},
	{"cd", "CD", HeaderRcv, 1, {{SONY, 0x520C, SONYBit, SONYRep, SONYDelay}}},

	{"tv power", "TV", HeaderPower, 1, {{NEC, 0x209D00FF, NECBit, NECRep, NECDelay}}},
	{"receiver power", "Receiver", HeaderPower, 1, {{SONY, 0x540c, SONYBit, SONYRep, SONYDelay}}},
	{"power", "All", HeaderPower, 2, {
  		{SONY, 0x540c, SONYBit, SONYRep, SONYDelay, 75}, // Delay of 75ms between commands
  		{NEC, 0x209D00FF, NECBit, NECRep, NECDelay}
    }
	},
};
