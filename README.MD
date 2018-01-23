# Photon IR Remote
The Photon's cloud services makes it easy to integrate with Alexa without the need to configure your router with port forwarding. Simply connect your photon to a wifi network with internet access.

Provides the following:
1. "Universal" remote with a web interface that can be used on a cell phone web page. Including Roku network api.
2. Can be controlled by a Amazon Alexa Skill.

While this code uses Alexa to control IR devices, it should be easy to extend this to handle anything you can handle with a Photon.

## Building for Particle
I use the Particle Desktop IDE (Dev) to compile and flash. When I first wrote this it that was not compatible with build.particle.io. I think they addressed that, but haven't tried.

# Setting up the controls
The buttons and Alexa commands are controlled by defining irActions. These are defined in IRAction header files to make it easy to switch between setups. To switch configs simply update photon-remote.ino to `#include` the correct header file.

#### IRCommand
The IRCommand struct defines a individual IR command to be sent. For example, Press the power button, or the up button.

#### IRAction
A IRAction is a collection of IRCommands. Each IRAction is represented as a button in the served web page. This allows you to trigger macros of several IRCommands with a single button press. In IRAction_Example.h you can see that the ps3 IRAction is navigating through the menu of my tv to pick the correct source(As my tv had no way to query its current source this only worked because I only switched between two sources, ps3 and computer). Another example would be to press the power button for both my tv and receiver.

IRAction_Roku.h provides a example of using wifi to communicate with a Roku device without using IR.

# Alexa
## Example commands

You can use single commands
* I say: Alexa, tell remote to press power
* I say: Alexa, tell remote to press volume up 3 times
* I say: Alexa, tell remote to press volume up 3

If you need to do multiple commands you can open the remote skill and have a conversation without needing to say Alexa several times. A example conversation:
```
I say: Alexa, open remote
Alexa says: Yes
I say: Press volume up
Alex says: Ok
I say: Press ps3
Alexa says: Ok
I say: thanks
Alexa says: Goodby
```

At some point I think integrating with the smart home api could be used to make it easier to use single commands, but I haven't experimented with using this yet.

## Setting up Alexa Skill

This skill **should only be used as a developer skill**, it's not setup to be usable outside of the developer tools. This was developed to use the Custom Interaction Model, but could be modified to work with the Smart Home Skill API to make the commands more natural.

**Important:** Make sure to use the same amazon account for AWS that the Echo device(s) is registered against. This allows you to enable it for your devices without needing to use the beta test program or release the skill. See https://developer.amazon.com/docs/custom-skills/test-a-custom-skill.html#h2_register

alexaSkill.py contains the settings I use for my configuration. Adjust the values to match your setup.

I'm summarizing the steps to create a new skill and lambda function below, I would recommend following this guide to creating a skill. https://developer.amazon.com/docs/custom-skills/steps-to-build-a-custom-skill.html I've included most of the values you need to store in the skill and the lambda function code in alexaSkill.py.

#### AWS Lambda

The AWS Lambda service allows you to run python code easily on the web and is simple to tie to the Alexa Skills service. The Alexa skill will call our lambda_handler and trigger commands on the Photon.

The example lambda_handler function should be run using python 2.7. The code is under the "AWS Lambda function" comment in alexaSkill.py. **Make sure to update the** `'<<insert device_id>>'` value with your Photon's device id(Expand the Photon ID by clicking on the > in build.particle.io under devices). You also **need to update** `'<<insert access_token>>'` with your account access token(Found under settings in build.particle.io). This is  private info, make sure to not share it with random people.

#### Alexa Skill
Login to https://developer.amazon.com/edw/home.html and click on "Getting Started >" for the "Alexa Skills Kit".

1. Create a "Custom Interaction Model".
2. Choose a invocation name that makes sense for you. I used remote, but this may not work well with other skills you may have installed.
3. Copy the contents of the string under the "Intent Schema" comment in alexaSkill.py.
4. Custom Slot Types: I am using the type "LIST_OF" for the answer type. This allows us to give alexa hits of the data we expect. I like to keep this up to date with all of the IRAction::name values used by the remote. You can find a example in the string under the "LIST_OF_ANSWERS" comment in alexaSkill.py.
5. Sample Utterances: This gives Alexa hints to how you will interact with the skill. For example you may say "Alexa tell remote to press volume up 3 times" and this will tell the photon to activate the "volumne up" IRAction 3 times.
