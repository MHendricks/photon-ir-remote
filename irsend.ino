/*
 * IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July,   2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include "IRremote.h"
#include "WebServer.h"
#include "neopixel.h"

//#include "IRAction_Example.h"
#include "IRAction_Mike.h"

/* Uncomment this line to enable using a OLED display. If not using one,
   leaving this defined will cause the Photon to lag and crash when the i2c
   device doesn't respond. */
//#define USE_OLED

// If defined, serve http://host/parsed.html this is useful for checking url args
//#define PARSEDCMD

#ifdef USE_OLED

  #include "Adafruit_GFX.h"
  #include "Adafruit_SSD1306.h"

  // OLED display --------------------------------------------------------------
  #define OLED_RESET 4
  Adafruit_SSD1306 display(OLED_RESET);

  #if (SSD1306_LCDHEIGHT != 64)
  #error("Height incorrect, please fix Adafruit_SSD1306.h!");
  #endif

  /* Clear the display when millis() is larger than this number. The display
     will not be cleared if this value is set to zero. */
  long lastUpdate = 0;
#endif

char deviceIP[24];

// IR Send ---------------------------------------------------------------------
IRsend irsend(D2);

#define button1 D3
#define button2 D7
/* Basic button state tracking. Only register a press/release when the button
   value is diffrent than these value. */
bool lastState1 = false;
bool lastState2 = false;
/* When processing a Particle.function call, the requested irAction and repeat is
    stored in these variables. This allows us to report if the request was valid,
    but we don't need to waste AWS processing time. */
int commandRequest = -1;
int commandRepeat = 0;

// Web Server ------------------------------------------------------------------
/* We will listen on port 80, the standard HTTP service port */
WebServer webserver("", 80);
#define NAMELEN 32
#define VALUELEN 1024

// NeoPixel --------------------------------------------------------------------
// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D6
#define PIXEL_COUNT 1
#define PIXEL_TYPE WS2811

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

uint8_t red = 0;
uint8_t green = 0;
uint8_t blue = 0;

/* Set the NeoPixel color to this RGB value */
void neoColor(int r, int g, int b) {
  strip.setPixelColor(0, r, g, b);
  strip.show();
}

/* Reset the NeoPixel to the color specified by the web server */
void neoReset() {
  neoColor(red, green, blue);
}

#ifdef USE_OLED
// Debug printing to OLED and serial console -----------------------------------
void displayClear(bool update=false) {
  // Clear the buffer.
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("IP:");
  display.print(deviceIP);
  display.setCursor(0, 16);
  if (update) {
    display.display();
  }
}

void displayPrint(String text, bool update=false) {
  display.print(text);
  Serial.print(text);
  if (update) {
    display.display();
  }
}

void displayPrintln(String text, bool update=false) {
  display.println(text);
  Serial.println(text);
  if (update) {
    display.display();
  }
}

void displayPrintln(int base, bool update=false) {
  display.println(base);
  Serial.println(base);
  if (update) {
    display.display();
  }
}
#endif

/* This command is set as the default command for the server.  It
 * handles both GET and POST requests.  For a GET, it returns a simple
 * page with some buttons.  For a POST, it saves the value posted to
 * the red/green/blue variable, affecting the output of the speaker */
void pageCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  if (type == WebServer::POST)
  {
    bool repeat;
    char name[16], value[16];
    do
    {
      /* readPOSTparam returns false when there are no more parameters
       * to read from the input.  We pass in buffers for it to store
       * the name and value strings along with the length of those
       * buffers. */
      repeat = server.readPOSTparam(name, 16, value, 16);
      if (repeat) {
        // Only send the name if we got a repeat value. This prevents sending
        // Two messages.
        Serial.println(name);
        sendIrAction(atoi(name));
      }
    } while (repeat);

    server.httpSuccess();

    return;
  }

  /* for a GET or HEAD, send the standard "it's all OK headers" */
  server.httpSuccess();

  /* we don't output the body for a HEAD request */
  if (type == WebServer::GET)
  {
    /* Python Code to convert a html file into P() compatible string data.
          prefix = '      '
          with open(r'.\example.html') as f:
          	for line in f.readlines():
          		line = line.replace('\n', '')
          		line = line.replace('"', '\\"')
          		print prefix + '"' + line + '\\n"'
    */

    /* store the HTML in program memory using the P macro */
    P(message) =
      "<!DOCTYPE html>\n"
      "<html>\n"
      "    <head>\n"
      "        <meta charset=\"utf-8\">\n"
      "        <meta name=\"apple-mobile-web-app-capable\" content=\"yes\" />\n"
      "        <meta name=\"apple-mobile-web-app-status-bar-style\" content=\"black\">\n"
      "        <link rel=\"apple-touch-icon\" href=\"/favicon.png\">\n"
      "        <link rel=\"apple-touch-startup-image\" href=\"/favicon.png\">\n"
      "        <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge,chrome=1\">\n"
      "        <meta name=\"viewport\" content=\"width=device-width, user-scalable=no\">\n"
      "        <title>Photon Blaster</title>\n"
      "        <link rel=\"stylesheet\" href=\"http://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.css\" />\n"
      "        <script src=\"http://code.jquery.com/jquery-2.1.4.min.js\"></script>\n"
      "        <script src=\"http://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.js\"></script>\n"
      "        <script src=\"//ajax.googleapis.com/ajax/libs/jquery/1.10.2/jquery.min.js\"></script>\n"
      "        <script src=\"//code.jquery.com/ui/1.11.4/jquery-ui.js\"></script>\n"
      "        <style>\n"
      "            .ui-body {-webkit-column-count: 2; /* Chrome, Safari, Opera */ -moz-column-count: 2; /* Firefox */ column-count: 2;}\n"
      "            .ui-btn-text, .ui-btn {position: initial;}\n"
      "            .ui-btn {display: inline-block;}\n"
      "            #red, #green, #blue { margin: 10px; } \n"
      "            #red { background: #f00; } \n"
      "            #green { background: #0f0; } \n"
      "            #blue { background: #00f; }\n"
      "            h3 {text-align: center;}\n"
      "            .ui-accordion-header {background-color:#748174; margin: 0 1em; width: initial; -webkit-margin-before: 0.25em; -webkit-margin-after: 0.25em;}\n"
      "            .ui-accordion-header-active {background-color:#487B41; margin: 0em;}\n"
      "        </style>\n"
      "        <script>\n"
      "            $(document).ready(function(){ \n"
      "                function changeRGB(event, ui) { \n"
      "                    /*not to DDoS the Arduino, you might have to change this to some threshold value that fits your setup*/\n"
      "                    jQuery.ajaxSetup({timeout: 110});  \n"
      "                    var id = $(this).attr('id');\n"
      "                    console.log(id + \"=\" + event.target.value);\n"
      "                    if (id == 'red') $.post('/rgb.html?r=' + event.target.value);\n"
      "                    if (id == 'green') $.post('/rgb.html?g=' + event.target.value);\n"
      "                    if (id == 'blue') $.post('/rgb.html?b=' + event.target.value);\n"
      "                };\n"
      "                $(':button').on('click', function(evt) {\n"
      "                    evt.preventDefault();\n"
      "                    if (this.id == 'Color') {\n"
      "                        console.log('/rgb.html?r=' + $('#red')[0].value + '&g=' + $('#green')[0].value + '&b=' + $('#blue')[0].value);\n"
      "                        $.post('/rgb.html?r=' + $('#red')[0].value + '&g=' + $('#green')[0].value + '&b=' + $('#blue')[0].value);\n"
      "                    } else {\n"
      "                        console.log(this.id);\n"
      "                        var data = {};\n"
      "                        data[this.id] = 0;\n"
      "                        $.post('', data);\n"
      "                    }\n"
      "                });\n"
      "                $('#red, #green, #blue').bind('change', changeRGB);\n"
      "            });\n"
      "            $(function() {\n"
      "                $( \"#accordion\" ).accordion({\n"
      "                    collapsible: true,\n"
      "                    heightStyle: \"content\"\n"
      "                });\n"
      "            });\n"
      "        </script>\n"
      "    </head>\n"
      "    <body>\n"
      "        <div data-theme=\"b\" data-role=\"page\" id=\"accordion\">\n";

    P(messageEnd) =
      "            </div>\n"
      "            <h3>Set Color</h3>\n"
      "            <div class=\"rgb-slider\">\n"
      "                <input type=\"range\" name=\"slider\" id=\"red\" value=\"0\" min=\"0\" max=\"255\" /><br>\n"
      "                <input type=\"range\" name=\"slider\" id=\"green\" value=\"0\" min=\"0\" max=\"255\" /><br>\n"
      "                <input type=\"range\" name=\"slider\" id=\"blue\" value=\"0\" min=\"0\" max=\"255\" /><br>\n"
      "                <button id=\"Color\">Apply</button>\n"
      "            </div>\n"
      "        </div>\n"
      "    </body>\n"
      "</html>\n";

    // Serve the first part of the web page
    server.printP(message);

    /* Multi-header support does not check if a header title was used before the
       current contents of lastHeader. So if you create actions with headers set
       in this order "A", "A", "B", "B", "B", "A" it will result in 3 headers
       "A", "B", "A" instead of "A", "B" */
    char *lastHeader = "";
    for (int i=0; i < ACTION_COUNT; i++) {
      IRAction action = irActions[i];
      if (lastHeader != action.header) {
        if (lastHeader != "") {
          server.printP("            </div>\n");
        }
        server.printP("            <h3>"); server.printP(action.header); server.printP("</h3>\n");
        server.printP("            <div class=\"ui-body\">\n");
        lastHeader = action.header;
      }

      server.printP("                <button type=\"button\" id=\"");
      server.printf("%i", i);
      server.printP("\">");
      server.printP(action.displayName);
      server.printP("</button>\n");
    }
    // Serve the last of the web page
    server.printP(messageEnd);
  }
}

void rgbCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
  URLPARAM_RESULT rc;
  char name[NAMELEN];
  char value[VALUELEN];

  /* this line sends the standard "we're all OK" headers back to the
     browser */
  server.httpSuccess();

  /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
  if (type == WebServer::HEAD)
    return;

  if (strlen(url_tail)) {
    while (strlen(url_tail)) {
      rc = server.nextURLparam(&url_tail, name, NAMELEN, value, VALUELEN);
      if (rc != URLPARAM_EOS) {
        if (strcmp(name, "r") == 0) {
          /* use the STRing TO Unsigned Long function to turn the string
           * version of the color strength value into our integer red/green/blue
           * variable */
          red = strtoul(value, NULL, 10);
        } else if (strcmp(name, "g") == 0) {
          green = strtoul(value, NULL, 10);
        } else if (strcmp(name, "b") == 0) {
          blue = strtoul(value, NULL, 10);
        }
      }
    }
    neoReset();
  }
}

#ifdef PARSEDCMD
#define VERSION_STRING "0.1"
P(Page_start) = "<html><head><title>Web_Parms_1 Version " VERSION_STRING "</title></head><body>\n";
P(Page_end) = "</body></html>";
P(Get_head) = "<h1>GET from ";
P(Post_head) = "<h1>POST to ";
P(Unknown_head) = "<h1>UNKNOWN request for ";
P(Default_head) = "unidentified URL requested.</h1><br>\n";
P(Raw_head) = "raw.html requested.</h1><br>\n";
P(Parsed_head) = "parsed.html requested.</h1><br>\n";
P(Good_tail_begin) = "URL tail = '";
P(Bad_tail_begin) = "INCOMPLETE URL tail = '";
P(Tail_end) = "'<br>\n";
P(Parsed_tail_begin) = "URL parameters:<br>\n";
P(Parsed_item_separator) = " = '";
P(Params_end) = "End of parameters<br>\n";
P(Post_params_begin) = "Parameters sent by POST:<br>\n";
P(Line_break) = "<br>\n";

void parsedCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  URLPARAM_RESULT rc;
  char name[NAMELEN];
  char value[VALUELEN];

  /* this line sends the standard "we're all OK" headers back to the
     browser */
  server.httpSuccess();

  /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
  if (type == WebServer::HEAD)
    return;

  server.printP(Page_start);
  switch (type)
    {
    case WebServer::GET:
        server.printP(Get_head);
        break;
    case WebServer::POST:
        server.printP(Post_head);
        break;
    default:
        server.printP(Unknown_head);
    }

    server.printP(Parsed_head);
    server.printP(tail_complete ? Good_tail_begin : Bad_tail_begin);
    server.print(url_tail);
    server.printP(Tail_end);

  if (strlen(url_tail))
    {
    server.printP(Parsed_tail_begin);
    while (strlen(url_tail))
      {
      rc = server.nextURLparam(&url_tail, name, NAMELEN, value, VALUELEN);
      if (rc == URLPARAM_EOS)
        server.printP(Params_end);
       else
        {
        server.print(name);
        server.printP(Parsed_item_separator);
        server.print(value);
        server.printP(Tail_end);
        }
      }
    }
  if (type == WebServer::POST)
  {
    server.printP(Post_params_begin);
    while (server.readPOSTparam(name, NAMELEN, value, VALUELEN))
    {
      server.print(name);
      server.printP(Parsed_item_separator);
      server.print(value);
      server.printP(Tail_end);
    }
  }
  server.printP(Page_end);

}
#endif

void faviconCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  /* this data was taken from a PNG file that was converted to a C data structure
   * by running it through the directfb-csource application. */
  P(ledData) = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
    0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x08, 0x02, 0x00, 0x00, 0x00, 0x90, 0x91, 0x68,
    0x36, 0x00, 0x00, 0x00, 0x01, 0x73, 0x52, 0x47, 0x42, 0x00, 0xae, 0xce, 0x1c, 0xe9, 0x00, 0x00,
    0x00, 0x04, 0x67, 0x41, 0x4d, 0x41, 0x00, 0x00, 0xb1, 0x8f, 0x0b, 0xfc, 0x61, 0x05, 0x00, 0x00,
    0x00, 0x20, 0x63, 0x48, 0x52, 0x4d, 0x00, 0x00, 0x7a, 0x26, 0x00, 0x00, 0x80, 0x84, 0x00, 0x00,
    0xfa, 0x00, 0x00, 0x00, 0x80, 0xe8, 0x00, 0x00, 0x75, 0x30, 0x00, 0x00, 0xea, 0x60, 0x00, 0x00,
    0x3a, 0x98, 0x00, 0x00, 0x17, 0x70, 0x9c, 0xba, 0x51, 0x3c, 0x00, 0x00, 0x00, 0x18, 0x74, 0x45,
    0x58, 0x74, 0x53, 0x6f, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x00, 0x50, 0x61, 0x69, 0x6e, 0x74,
    0x2e, 0x4e, 0x45, 0x54, 0x20, 0x76, 0x33, 0x2e, 0x33, 0x36, 0xa9, 0xe7, 0xe2, 0x25, 0x00, 0x00,
    0x00, 0x57, 0x49, 0x44, 0x41, 0x54, 0x38, 0x4f, 0x95, 0x52, 0x5b, 0x0a, 0x00, 0x30, 0x08, 0x6a,
    0xf7, 0x3f, 0xf4, 0x1e, 0x14, 0x4d, 0x6a, 0x30, 0x8d, 0x7d, 0x0d, 0x45, 0x2d, 0x87, 0xd9, 0x34,
    0x71, 0x36, 0x41, 0x7a, 0x81, 0x76, 0x95, 0xc2, 0xec, 0x3f, 0xc7, 0x8e, 0x83, 0x72, 0x90, 0x43,
    0x11, 0x10, 0xc4, 0x12, 0x50, 0xb6, 0xc7, 0xab, 0x96, 0xd0, 0xdb, 0x5b, 0x41, 0x5c, 0x6a, 0x0b,
    0xfd, 0x57, 0x28, 0x5b, 0xc2, 0xfd, 0xb2, 0xa1, 0x33, 0x28, 0x45, 0xd0, 0xee, 0x20, 0x5c, 0x9a,
    0xaf, 0x93, 0xd6, 0xbc, 0xdb, 0x25, 0x56, 0x61, 0x01, 0x17, 0x12, 0xae, 0x53, 0x3e, 0x66, 0x32,
    0xba, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
  };

  if (type == WebServer::POST)
  {
    // ignore POST data
    server.httpFail();
    return;
  }

  /* for a GET or HEAD, send the standard "it's all OK headers" but identify our data as a PNG file */
  server.httpSuccess("image/png");

  /* we don't output the body for a HEAD request */
  if (type == WebServer::GET)
  {
    server.writeP(ledData, sizeof(ledData));
  }
}

int sendCommand(String command) {
  #ifdef USE_OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    displayPrintln("Command:");
    displayPrint(command);
    displayPrintln("--", true);
  #endif
  //Serial.print("---"); Serial.print(command);Serial.println("---");
  /* Particle.function only accepts a single argument so to support repeat counts
      we look for a :. "volume up:3" For example would send "volume up" 3 times. */
  char buf[63];
  command.toCharArray(buf, 63);
  char* chunk = strtok(buf, ":");
  commandRequest = findCommandId(chunk);
  if (commandRequest == -1) {
    //Serial.println("Bad Request.");
    // Bad Request notify the caller.
    return -1;
  }
  // Look for a repeat count argument. If not found force the repeat count to 1.
  commandRepeat = atoi(strtok(0, ":"));
  if (commandRepeat == 0) {
    commandRepeat = 1;
  }
  //Serial.print("*"); Serial.print(commandRequest); Serial.print("*"); Serial.print(commandRepeat);  Serial.println("*");
  return 1;
}

void setup()
{
  /* Get the IP address so we can publish it as a spark core and show it in the
    OLED display if connected */
  IPAddress myIp = WiFi.localIP();
  sprintf(deviceIP, "%d.%d.%d.%d", myIp[0], myIp[1], myIp[2], myIp[3]);
  /* Store the ipAddress as a Spark Variable so we can get the IP address
     without using a OLED display */
  Particle.variable("ipAddress", deviceIP, STRING);
  // Add a test function to cloud
  //Particle.function("setLed", setLed);
  Particle.function("sendCommand", sendCommand);

  Serial.begin(9600);

  pinMode(button1, INPUT_PULLDOWN);
  pinMode(button2, INPUT_PULLDOWN);

  #ifdef USE_OLED
    // OLED ----------------------------------------------------------
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
    // Show the IP address
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.display();
    display.clearDisplay();
    display.setTextSize(2);
    display.println("");
    display.setTextSize(3);
    display.println("IR");
    display.println("Blaster");
    display.display();
  #endif

  // Web Server ----------------------------------------------------------
  // register our default command (activated with the request of http://x.x.x.x
  webserver.setDefaultCommand(&pageCmd);
  webserver.addCommand("favicon.png", &faviconCmd);
  webserver.addCommand("rgb.html", &rgbCmd);
  #ifdef PARSEDCMD
    webserver.addCommand("parsed.html", &parsedCmd);
  #endif

  /* start the server to wait for connections */
  webserver.begin();

  /* NeoPixel setup */
  strip.begin();
  neoColor(0, 0, 0);

  #ifdef USE_OLED
    // Show the Program name for a while, but continue to boot.
    lastUpdate = millis();
  #endif
}

void loop() {
  // process incoming connections from the web server
  webserver.processConnection();

  // Process Particle.function requests if provided
  if (commandRequest != -1) {
    do {
      if (commandRepeat--) {
        // Only send the name if we got a repeat value. This prevents sending
        // Two messages.
        Serial.println(irActions[commandRequest].name);
        sendIrAction(commandRequest, true);
      }
    } while (commandRepeat);
    commandRequest = -1;
  }

  // Check if button1 was pressed
  bool state = digitalRead(button1);
  if (state != lastState1) {
      if (state) {
        sendIrAction(BUTTON1_ACTION_ID);
      }
      lastState1 = state;
  }

  // Check if button2 was pressed
  state = digitalRead(button2);
  if (state != lastState2) {
      if (state) {
        sendIrAction(BUTTON2_ACTION_ID);
      }
      lastState2 = state;
  }

  #ifdef USE_OLED
    // Clear the display if the timer has expired
    if (lastUpdate != 0) {
      unsigned long currentMillis = millis();
      if(currentMillis - lastUpdate > displayReset) {
        displayClear(true);
        lastUpdate = 0;
      }
    }
  #endif
}

/* Convert a string argument to a command index */
int findCommandId(String command) {
  for(int i=0; i < ACTION_COUNT; i++) {
    if (command.equalsIgnoreCase(irActions[i].name)) {
      return i;
    }
  }
  return -1;
}

bool sendIrAction(int actionId) {
  sendIrAction(actionId, false);
}

bool sendIrAction(int actionId, bool forceDelay) {
  // Reset the display after the delay
  #ifdef USE_OLED
    /*lastUpdate = millis();
    displayClear();
    display.setTextSize(2);*/
  #endif
  if (actionId < 0 || actionId >= ACTION_COUNT) {
    // A invalid id was passed, blink red 3 times then return.
    /*#ifdef USE_OLED
      displayPrint("Invalid ID");
      displayPrintln(actionId, true);
    #endif*/
    for (int i=0; i < 3; i++) {
      neoColor(255, 0, 0);
      delay(100);
      neoColor(0, 0, 0);
      delay(100);
    }
    neoReset();
    return false; // Invalid value
  }

  /*#ifdef USE_OLED
    displayPrintln("Sending:");
    displayPrint(irActions[actionId].displayName, true);
  #endif*/
  for (int i=0; i < irActions[actionId].commandCount; i++) {
    // Send each command
    IRCommand command = irActions[actionId].commands[i];
    sendMessage(command.sysType, command.data, command.nbits, command.repeat, command.repeatDelay);
    /* Only delay by IRCommand.delay if there are more commands to process.
      this allows us to reuse IRCommands with delay set, and doesn't tie up
      the process for a additional delay. */
    if (forceDelay || i+1 < irActions[actionId].commandCount) {
      // Show a open ended progress bar of each command that is sent
      /*#ifdef USE_OLED
        display.setTextSize(1);
        display.setCursor(i, SSD1306_LCDHEIGHT - 8); // 8 is the base height of text
        displayPrintln("|", true);
      #endif*/

      // Delay between commands
      delay(command.delay);

      #ifdef USE_OLED
        // Ensure the display doesn't reset until after all of the commands have processed
        lastUpdate = millis();
      #endif
    }
  }
  return true;
}

void sendMessage(int system, unsigned long data, int nbits, int repeat, int repeatDelay) {
    neoColor(0, 0, 25);
    for (int i=0; i < repeat; i++) {
      switch (system) {
        case SONY:
          irsend.sendSony(data, nbits);
          break;
        case NEC:
          irsend.sendNEC(data, nbits);
          break;
        case RC5:
          irsend.sendRC5(data, nbits);
          break;
        case RC6:
          irsend.sendRC6(data, nbits);
          break;
        case SHARP:
          irsend.sendSharp(data, nbits);
          break;
        case DISH:
          irsend.sendDISH(data, nbits);
          break;
        case PANASONIC:
          irsend.sendPanasonic(data, nbits);
          break;
        default:
          break;
      }
      delay(repeatDelay);
    }
    neoReset();
}
