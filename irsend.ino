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

#include "example.h"

// IR Send ----------------------------------------------------------------------------------------------
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
String configData = ""; // Used to store the config string

IRsend irsend(D2);
//IRrecv irrecv(D5);

#define button D3
bool lastState = false;

// Web Server -------------------------------------------------------------------------------------------
/* We will listen on port 80, the standard HTTP service port */
WebServer webserver("", 80);
#define NAMELEN 32
#define VALUELEN 1024
// If defined, accept http://host/parsed.html for debuging
#define PARSEDCMD

// NeoPixel ---------------------------------------------------------------------------------------------
// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D6
#define PIXEL_COUNT 1
#define PIXEL_TYPE WS2811

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

uint8_t red = 0;
uint8_t green = 0;
uint8_t blue = 0;

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
      Serial.println(name);

      sendIrAction(atoi(name));

    } while (repeat);

    server.httpSuccess();

    return;
  }

  /* for a GET or HEAD, send the standard "it's all OK headers" */
  server.httpSuccess();

  /* we don't output the body for a HEAD request */
  if (type == WebServer::GET)
  {
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
        "        <link rel=\"stylesheet\" href=\"http://code.jquery.com/mobile/1.0/jquery.mobile-1.0.min.css\" />\n"
        "        <script src=\"http://code.jquery.com/jquery-1.6.4.min.js\"></script>\n"
        "        <script src=\"http://code.jquery.com/mobile/1.0/jquery.mobile-1.0.min.js\"></script>\n"
        "        <style> body, .ui-page { background: black; } .ui-body { padding-bottom: 1.5em; }</style>\n"
        "        <script src=\"//ajax.googleapis.com/ajax/libs/jquery/1.10.2/jquery.min.js\"></script>\n"
        "        <script>\n"
        "            $(document).ready(function(){ \n"
        "                $(':button').on('click', function(evt) {\n"
        "                    evt.preventDefault();\n"
        "                    console.log(this.id);\n"
        "                    var data = {};\n"
        "                    data[this.id] = 0;\n"
        "                    $.post('', data);\n"
        "                });\n"
        "                $('a[href=\"#upload_config\"]').click(function(){\n"
        "                    var configString = prompt(\"Please paste your config\", \"idName,DisplayName,system,code,nBits;\");\n"
        "                    if (configString != null) {\n"
        "                        console.log(configString);\n"
        "                        $.post('setConfig.html', {'config':configString});\n"
        "                    }\n"
        "                }); \n"
        "            });\n"
        "        </script>\n"
        "    </head>\n"
        "    <body>\n"
        "        <div data-role=\"header\" data-position=\"inline\">\n"
        "            <h1>Photon Blaster</h1>\n"
        "        </div>\n"
        "        <div class=\"ui-body ui-body-a\">\n";

    P(messageEnd) =
        "        </div>\n"
        "    </body>\n"
        "</html>\n";

    // Serve the first part of the web page
    server.printP(message);

    for (int i=0; i<BUTTON_COUNT; i++) {
      server.printP("            <button type=\"button\" id=\"");
      server.printf("%i", i);
      server.printP("\">");
      server.printP(irActions[i].displayName);
      server.printP("</button>\n");
    }
    // Serve the last of the web page
    server.printP(messageEnd);
  }
}

void setConfigCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
  URLPARAM_RESULT rc;
  char name[NAMELEN];
  char value[VALUELEN];

  server.httpSuccess();

  if (type == WebServer::POST) {
    while (server.readPOSTparam(name, NAMELEN, value, VALUELEN))
    {
      Serial.print("Name: ");
      Serial.println(name);
      Serial.print("Value: ");
      Serial.println(value);
      if (strcmp(name, "config") == 0) {
        Serial.println("Its config");
        configData = value;
      }
    }
  }
}

void setConfigCmdTest(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
  URLPARAM_RESULT rc;
  char name[NAMELEN];
  char value[VALUELEN];

  if (!tail_complete) {
    // this line sends the standard "we're all OK" headers back to the browser
    Serial.println("URL Tail not complete");
    server.httpFail();
  } else {
    Serial.println("URL Tail complete");
    server.httpSuccess();
  }

  if (strlen(url_tail)) {
      Serial.println(url_tail);
      while (strlen(url_tail)) {
        rc = server.nextURLparam(&url_tail, name, NAMELEN, value, VALUELEN);
        if (rc == URLPARAM_EOS) {
          Serial.println("OK");
        } else {
          Serial.println(name);
          Serial.println(value);
          if (strcmp(name, "config") == 0) {
            configData = value;
            return;
          }
        }
      }
    }
  configData = "Fail";
  return ;

  if (type == WebServer::POST)
  {
    bool repeat;
    char name[16], value[1024];
    do
    {
      /* readPOSTparam returns false when there are no more parameters
       * to read from the input.  We pass in buffers for it to store
       * the name and value strings along with the length of those
       * buffers. */
      repeat = server.readPOSTparam(name, 16, value, 16);
      Serial.println(name);

      //if (name == "config") {
      configData = "Value";
      //}

    } while (repeat);

    server.httpSuccess();
    return;
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
        strip.setPixelColor(0, red, green, blue);
        strip.show();
      }
    }
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

void setup()
{
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  pinMode(button, INPUT_PULLDOWN);
  pinMode(D7, OUTPUT);

  // Web Server ----------------------------------------------------------
  /* register our default command (activated with the request of
   * http://x.x.x.x/rgb */
  webserver.setDefaultCommand(&pageCmd);
  webserver.addCommand("setConfig.html", &setConfigCmd);
  webserver.addCommand("favicon.png", &faviconCmd);
  webserver.addCommand("rgb.html", &rgbCmd);
  #ifdef PARSEDCMD
  webserver.addCommand("parsed.html", &parsedCmd);
  #endif

  /* start the server to wait for connections */
  webserver.begin();

  /* NeoPixel setup */
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
    // process incoming connections one at a time forever
    webserver.processConnection();

  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      // add it to the inputString:
      inputString += inChar;
    }
  }
  bool state = digitalRead(button);
  if (state != lastState) {
      if (state){
        stringComplete = true;
        inputString = "tvMenu";
      }
      lastState = state;
  }
  // print the string when a newline arrives:
  if (stringComplete) {
    translateIrCode(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

void translateIrCode(String inputString){

    Serial.println(inputString);
}

bool sendIrAction(int actionId) {
  if (actionId < 0 || actionId > BUTTON_COUNT) {
    return false; // Invalid value
  }
  for (int i; i < DATA_COUNT; i++) {
    IRData command = irActions[actionId].commands[0];
    sendMessage(command.sysType, command.data, command.nbits);
  }
}

void sendMessage(int system, unsigned long data, int nbits) {
    digitalWrite(D7, HIGH);
    strip.setPixelColor(0, 0, 0, 255);
    strip.show();
    if (system == SONY) {
        for (int i = 0; i < 3; i++) {
          irsend.sendSony(data, nbits); // Sony
          delay(40);
        }
    } else if (system == NEC) {
        for (int i = 0; i < 1; i++) {
          irsend.sendNEC(data, nbits); // Sony
          delay(40);
        }
    }
    digitalWrite(D7, LOW);
    strip.setPixelColor(0, red, green, blue);
    strip.show();
}
