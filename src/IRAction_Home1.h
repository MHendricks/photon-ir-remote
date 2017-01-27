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
#define HeaderGen "General"

IRAction irActions[ACTION_COUNT] = {
	{"power", "Power", HeaderGen, 1, {{NEC, 0x61A0F00F, NECBit, NECRep, NECDelay}}},
	{"computer", "Computer", HeaderGen, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "launch/tvinput.hdmi1?contentID=tvinput.hdmi1"}}},
	{"chromecast", "Chromecast", HeaderGen, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "launch/tvinput.hdmi3?contentID=tvinput.hdmi3"}}},
	{"volume up", "Volume Up", HeaderGen, 1, {{NEC, 0x61A0F00F, NECBit, NECRep, NECDelay}}},
	{"volume down", "Volume Dn", HeaderGen, 1, {{NEC, 0x61A0F00F, NECBit, NECRep, NECDelay}}}
};

// Roku setup
#define USE_ROKU
byte roku_server[] = { 192, 168, 1, 116 };
int roku_port = 8060;
