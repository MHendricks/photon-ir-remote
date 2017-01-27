/* Example of the config header used to define all of the actions the IR Blaster
	will show on its web page and the ir commands it sends when the buttons are
	pressed in the web page and on the device.
 */
#include "IRAction.h"
int const ACTION_COUNT = 20;
int const BUTTON1_ACTION_ID = 14;
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
IRCommand tvPower = {NEC, 0x61A0F00F, NECBit, NECRep, NECDelay};
IRCommand tvInputComputer = {ROKU_REST, 0, 0, 1, 0, 0, "launch/tvinput.hdmi1?contentID=tvinput.hdmi1"};
IRCommand tvInputPlayStation = {ROKU_REST, 0, 0, 1, 0, 0, "launch/tvinput.hdmi2?contentID=tvinput.hdmi2"};

IRCommand rcvInputMulti = {SONY, 0x270C, SONYBit, SONYRep, SONYDelay};
IRCommand rcvInputVideo2 = {SONY, 0x3C0C, SONYBit, SONYRep, SONYDelay};

IRAction irActions[ACTION_COUNT] = {
	{"volume up", "Volume Up", HeaderGen, 1, {{SONY, 0x240C, SONYBit, SONYRep, SONYDelay, SONYRptDelay}}},
	{"volume down", "Volume Dn", HeaderGen, 1, {{SONY, 0x640C, SONYBit, SONYRep, SONYDelay, SONYRptDelay}}},
	{"mute", "Mute", HeaderGen, 1, {{SONY, 0x140C, SONYBit, SONYRep, SONYDelay}}},
	{"ps3", "PS3", HeaderGen, 2, {
			rcvInputVideo2,
      tvInputPlayStation
    }
  },
	{"computer", "Computer", HeaderGen, 2, {
			rcvInputMulti,
      tvInputComputer
    }
  },

	{"video 1", "Video 1", HeaderRcv, 1, {{SONY, 0x220C, SONYBit, SONYRep, SONYDelay}}},
	{"video 2", "Video 2", HeaderRcv, 1, {rcvInputVideo2}},
	{"dvd", "DVD", HeaderRcv, 1, {{SONY, 0x5F0C, SONYBit, SONYRep, SONYDelay}}},
	{"tuner", "Tuner", HeaderRcv, 1, {{SONY, 0x420C, SONYBit, SONYRep, SONYDelay}}},
	{"multi channel", "Multi Ch", HeaderRcv, 1, {rcvInputMulti}},
	{"tape", "Tape", HeaderRcv, 1, {{SONY, 0x4B0C, SONYBit, SONYRep, SONYDelay}}},
	{"cd", "CD", HeaderRcv, 1, {{SONY, 0x520C, SONYBit, SONYRep, SONYDelay}}},

	{"tv power", "TV", HeaderPower, 1, {tvPower}},
	{"receiver power", "Receiver", HeaderPower, 1, {{SONY, 0x540c, SONYBit, SONYRep, SONYDelay}}},
	{"power", "All", HeaderPower, 2, {
			{SONY, 0x540c, SONYBit, SONYRep, SONYDelay, 75}, // Delay of 75ms between commands
			tvPower
    }
	},
};

// Roku setup
#define USE_ROKU
byte roku_server[] = { 192, 168, 1, 116 };
int roku_port = 8060;
