//joystick processing library
#include <Arduino.h>
#include "joystickHelper.h"

joystickHelper::joystickHelper(int xPin, int yPin, int swPin) {
  joystickHelper::xPin = (int)xPin;
  joystickHelper::yPin = (int)yPin;
  joystickHelper::swPin = (int)swPin;

  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(swPin, INPUT_PULLUP); //use internal pullup reistors from arduino
}

boolean joystickHelper::isPressed() {
  joystickHelper::update();
  return joystickHelper::pressed;
}

joystickPosition joystickHelper::getPosition() {
  joystickHelper::update();
  return {x: joystickHelper::x, y: joystickHelper::y};
}

void joystickHelper::update() {
  //STEP 1: deal with button (debounce logic)
  /*int swReading = digitalRead(joystickHelper::swPin);
  if (swReading != joystickHelper::lastPressState) { //debouncing stuff
    joystickHelper::lastPressTime = millis();
  }
  if ((millis() - joystickHelper::lastPressTime) > 50) { //50ms debounce delay
    if (swReading != joystickHelper::pressed) {
      joystickHelper::pressed = swReading;
    }
  }*/
  joystickHelper::pressed = (digitalRead(joystickHelper::swPin) == LOW);

  //STEP 2: deal with xPos
  int xReading = analogRead(joystickHelper::xPin);
  joystickHelper::x = map(xReading, 0, 1023, -100, 100); //map to percentages

  //STEP 3: deal with yPos
  int yReading = analogRead(joystickHelper::yPin);
  joystickHelper::y = map(yReading, 0, 1023, -100, 100);
}
  
