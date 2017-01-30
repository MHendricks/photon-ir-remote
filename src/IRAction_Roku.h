/* Example of the config header used to define all of the actions the IR Blaster
	will show on its web page and the ir commands it sends when the buttons are
	pressed in the web page and on the device.
 */
#include "IRAction.h"
int const ACTION_COUNT = 29;
int const BUTTON1_ACTION_ID = 14;
int const BUTTON2_ACTION_ID = 1;
const long displayReset = 10000;

#define NECBit 32
#define NECRep 1
#define NECDelay 40
#define NECRptDelay 300
#define HeaderNav "Navigation"
#define HeaderSound "Sound"
#define HeaderApps "Apps"
#define HeaderPlay "Playback"
#define HeaderMisc "Misc"

IRAction irActions[ACTION_COUNT] = {
	// TODO: Add nice layout for DPad
	// Column 1
	{"back", "<- Back", HeaderNav, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/Back"}}},
	{"left", "Left", HeaderNav, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/Left"}}},
	{"up", "Up", HeaderNav, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/Up"}}},
	{"down", "Down", HeaderNav, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/Down"}}},
	// Column 2
	{"Home", "Home", HeaderNav, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/Home"}}},
	{"right", "Right", HeaderNav, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/Right"}}},
	{"okay", "OK", HeaderNav, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/Select"}}},

	{"volume up", "Volume Up", HeaderSound, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/VolumeUp"}}},
	{"volume down", "Volume Down", HeaderSound, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/VolumeDown"}}},
	{"mute", "Mute", HeaderSound, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/VolumeMute"}}},

	{"rewind", "Rewind", HeaderPlay, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/Rev"}}},
	{"replay", "Replay", HeaderPlay, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/InstantReplay"}}},
	// TODO: Add support for voice words that are not displayed like pause
	{"pause", "", HeaderPlay, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/Play"}}},
	{"fast forward", "Fast Forward", HeaderPlay, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/Fwd"}}},
	{"play", "Play", HeaderPlay, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/Play"}}},

	{"power", "Power", HeaderMisc, 1, {{NEC, 0x61A0F00F, NECBit, NECRep, NECDelay}}}, // My tv turns off wifi after 15 minutes so use IR
	{"power off", "Power Off", HeaderMisc, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/PowerOff"}}},
	{"enter", "Enter", HeaderMisc, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/Enter"}}},
	{"star", "*", HeaderMisc, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/Info"}}},
	{"sleep", "Sleep", HeaderMisc, 1, {{NEC, 0x61a0c837, NECBit, NECRep, NECDelay}}}, // No rest command

	{"computer", "Computer", HeaderApps, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/InputHDMI1"}}},
	{"google", "Google Play", HeaderApps, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "launch/50025?contentID=50025"}}},
	{"google play", "", HeaderApps, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "launch/50025?contentID=50025"}}},
	{"amazon", "Amazon", HeaderApps, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "launch/13?contentID=13"}}},
	{"netflix", "Netflix", HeaderApps, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "launch/12?contentID=12"}}},

	{"chromecast", "Chromecast", HeaderApps, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "keypress/InputHDMI3"}}},
	{"hbo", "HBO", HeaderApps, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "launch/61322?contentID=61322"}}},
	{"crunchyroll", "Crunchyroll", HeaderApps, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "launch/2595?contentID=2595"}}},
	{"youtube", "YouTube", HeaderApps, 1, {{ROKU_REST, 0, 0, 1, 0, 0, "launch/837?contentID=837"}}},
};

// Roku setup
#define USE_ROKU
byte roku_server[] = { 192, 168, 1, 116 };
int roku_port = 8060;
