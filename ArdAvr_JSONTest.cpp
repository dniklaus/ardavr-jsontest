/*
  Sample sketch communicating over Serial using JSON

  This sketch communicates over serial link with a computer, sending
  JSON-encoded state of its analog pints every second and accepting
  per-line JSON messages with desired values of PWM pins on input.

  Circuit:
  * (Optional) Analog sensors attached to analog pin.
  * (Optional) LEDs attached to PWM pins 9 and 8.

  created 1 November 2012
  by Petr Baudis

  https://github.com/interactive-matter/ajson
  This code is in the public domain.
 */

// Do not remove the include below
#include "ArdAvr_JSONTest.h"

#include <aJSON.h>

unsigned long last_print = 0;
aJsonStream serial_stream(&Serial);

void setup()
{
  Serial.begin(115200);
}

/* Generate message like: { "analog": [0, 200, 400, 600, 800, 1000] } */
aJsonObject *createMessageToSend()
{
  aJsonObject *msg = aJson.createObject();

  int analogValues[6];
  for (int i = 0; i < 6; i++) {
    analogValues[i] = analogRead(i);
  }
  aJsonObject *analog = aJson.createIntArray(analogValues, 6);
  aJson.addItemToObject(msg, "analog", analog);

  return msg;
}

/* Process message like: {"straight":{"distance": 10,"topspeed": 100},"turn":{"angle": 45},"emergencyStop":{"stop":false}}*/
void processLintillaMessageReceived(aJsonObject *msg)
{
  bool emergencyStopValue = false;
  int topspeed = 0;
  int distanceValue = 0;
  int angleValue = 0;

  //TODO Make the following more generic

  aJsonObject *straight = aJson.getObjectItem(msg, "straight");
  aJsonObject *turn = aJson.getObjectItem(msg, "turn");
  aJsonObject *emergencyStop = aJson.getObjectItem(msg, "emergencyStop");
  if (!straight || !turn || !emergencyStop)
  {
    Serial.println("no lintella data");
    return;
  }

  aJsonObject *straightSpeed = aJson.getObjectItem(straight, "topspeed");
  Serial.println(straightSpeed->name);

  if (straightSpeed)
  {
    if (straightSpeed->type == aJson_Int)
    {
      topspeed = straightSpeed->valueint;
      Serial.println("topspeed: ");
      Serial.println(topspeed);
    }
    else
    {
      Serial.println("invalid topspeed data type ");
    }
  }
  else
  {
    Serial.println("invalid topspeed data");
  }

  aJsonObject *distance = aJson.getObjectItem(straight, "distance");
  Serial.println(distance->name);

  if (distance)
  {
    if (distance->type == aJson_Int)
    {
      distanceValue = distance->valueint;
      Serial.println("distance: ");
      Serial.println(distanceValue);
    }
    else
    {
      Serial.println("invalid distance data type ");
    }
  }
  else
  {
    Serial.println("invalid distance data");
  }

  aJsonObject *angle = aJson.getObjectItem(turn, "angle");
  Serial.println(angle->name);

  if (angle)
  {
    if (angle->type == aJson_Int)
    {
      angleValue = angle->valueint;
      Serial.println("angle: ");
      Serial.println(angleValue);
    }
    else
    {
      Serial.println("invalid angle data type ");
    }
  }
  else
  {
    Serial.println("invalid angle data");
  }

  //TODO Make the boolean working
  aJsonObject *emergency = aJson.getObjectItem(turn, "stop");
  Serial.println(emergency->name);
  Serial.println(emergency->valuebool);

  if (emergency->type == aJson_True)
  {
    emergencyStopValue = emergency->valuebool;
    Serial.println("emergency: ");
    Serial.println(emergencyStopValue);
  }
  else
  {
    Serial.println("invalid emergency data type ");
  }
}

void loop()
{
  if (millis() - last_print > 1000) {
    /* One second elapsed, send message. */
    aJsonObject *msg = createMessageToSend();
    aJson.print(msg, &serial_stream);
    Serial.println(); /* Add newline. */
    aJson.deleteItem(msg);
    last_print = millis();
  }

  if (serial_stream.available()) {
    /* First, skip any accidental whitespace like newlines. */
    serial_stream.skip();
  }

  if (serial_stream.available()) {
    /* Something real on input, let's take a look. */
    aJsonObject *msg = aJson.parse(&serial_stream);
    processLintillaMessageReceived(msg);
    aJson.deleteItem(msg);
  }
}
