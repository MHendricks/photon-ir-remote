# This document's the alexa skill I'm using to control my photon. Make sure to fill in device id and access token.

# Intent Schema
"""
{
  "intents": [
   {
      "intent": "AnswerIntent",
      "slots": [
        {
          "name": "Answer",
          "type": "LIST_OF_ANSWERS"
        },
        {
          "name": "Number",
          "type": "AMAZON.NUMBER"
        }
      ]
    },
    {
      "intent": "AMAZON.HelpIntent"
    },
    {
      "intent": "AMAZON.StopIntent"
    }
  ]
}
"""
# LIST_OF_ANSWERS: Fill this list with your commands to provide hints for AnswerIntent
"""
volume up
volume down
mute
ps3
computer
source
menu
enter
up
down
left
right
video 1
video 2
dvd
tuner
multi channel
tape
cd
tv power
receiver power
power
okay
back
home
replay
rewind
fast forward
play
pause
star
sleep
google
netflix
chromecast
"""

# Sample Utterances
"""
AnswerIntent press {Answer} {Number}
AnswerIntent press {Answer} {Number} times
AnswerIntent switch to {Answer}
AMAZON.StopIntent thanks
AMAZON.StopIntent thank you
AMAZON.StopIntent stop
AMAZON.StopIntent no
"""

# AWS Lambda function
import logging
logger = logging.getLogger()
logger.setLevel(logging.INFO)

import json
import urllib
import urllib2

device_id = '<<insert device_id>>'
access_token = '<<insert access_token>>'

def lambda_handler(event, context):
    logger.info(json.dumps(event))
    intentName = event['request'].get('intent', {}).get('name', {})
    if intentName in ("AMAZON.StopIntent", "AMAZON.CancelIntent"):
        return buildResponse('Good bye!', 'Please say remote command', True)
    if event['request'].get('type') == "LaunchRequest":
        return buildResponse('Yes?', 'Please say remote command', singleRequest=False)
    elif event['request'].get('type') == "IntentRequest":
        answer = event['request'].get('intent', {}).get('slots', {}).get('Answer', {}).get('value', '').lower()
        number = event['request'].get('intent', {}).get('slots', {}).get('Number', {}).get('value')
        if answer:
            logger.info([number])
            try:
                # Ensure this is a valid value
                int(number)
                answer = ':'.join([answer, number])
                logger.info([answer])
            except (TypeError, ValueError):
                logger.info('Bad Request');
            url = 'https://api.particle.io/v1/devices/{device_id}/sendCommand'.format(device_id=device_id)
            values = {
                'arg' : answer,
            	'access_token' : access_token,
            }

            data = urllib.urlencode(values)
            req = urllib2.Request(url, data)
            response = urllib2.urlopen(req)
            the_page = response.read()
            logger.info(the_page)
            success = json.loads(the_page).get('return_value', -1)
        else:
            success = False
        singleRequest = event.get('session', {}).get("attributes", {}).get("singleRequest", 'true')
        logger.info([singleRequest])
        singleRequest = singleRequest == 'true'
        if success == 1:
            text = "No Problem" if singleRequest else "OK"
        else:
            text = "Command {} not understood".format(answer)
        return buildResponse(text, endSession=singleRequest, singleRequest=singleRequest);

def strBool(state):
    return str(state).lower()

def buildResponse(output, reprompt='Anything else?', endSession=False, singleRequest=True):
    ret = {
        "version": "1.0",
        "response": {
        "outputSpeech": {
            "type": "PlainText",
            "text": output
        },
        "reprompt": {
            "outputSpeech": {
                "type": "PlainText",
                "text": reprompt
            }
        },
        "shouldEndSession": strBool(endSession)
        },
        "sessionAttributes": {
            "singleRequest": strBool(singleRequest)
        }
    }
    logger.info(ret)
    return ret
