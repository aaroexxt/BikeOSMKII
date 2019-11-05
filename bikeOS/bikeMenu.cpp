#include <Arduino.h>
#include "bigFont.h"
#include "bikeMenu.h"
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include "joystickHelper.h" //joystick library

bikeMenu::bikeMenu(String *menuStates, int states, LiquidCrystal_I2C & lcd) : lcdRef (lcd){ //Initialize references
  _menuStates = menuStates;
  _numberOfStates = states;

    //Menu Offsetting
  _menuOffset = 0;
  _cursorOffset = 0; //cursor offset from top
  _currentState = 0;
}

int bikeMenu::getOffset() {
  return _menuOffset + _cursorOffset;
}

void bikeMenu::renderMenu() {
  lcdRef.clear();
  lcdRef.setCursor(0, 0);
  lcdRef.print("MENU --------------");
  int counter = _menuOffset;
  for (int i = 0; i < 3; i++) { //only paint 3 rows (lcd height 4, but top row for menu header)
    if (counter > _numberOfStates) {
      counter = 0; //wrap it back around to print original states
    } else if (counter < 0) { //add number of states to wrap around (really shouldn't happen but in case it does)
      counter += _numberOfStates;
    }
    if (i == _cursorOffset) { //is the current row the row with the cursor
      lcdRef.setCursor(0, i+1); //must add 1 because row count starts from 1
      lcdRef.write(126); //print the right arrow
    }
    lcdRef.setCursor(1, i+1);
    lcdRef.print(_menuStates[counter].substring(0, 19)); //substring it so as to make it not break the screen logic
    counter++;
  }
}
/*
 * CURSOR OFFSET | MENU OFFSET
 * 0 0 (pressing down)
 * 1 0
 * 2 0
 * 2 1
 * 2 2
 * 2 3
 * 2 4
 * 2 3 (pressing up)
 * 2 2
 * 2 1
 * 2 0
 * 1 0
 * 0 0
 * 0 14
 */

int bikeMenu::changeMenuPosition(int change) {
  _cursorOffset += change;
  if (_cursorOffset > 2) { //cursor is offscreen below
    _menuOffset += calculateMenuOffset(_cursorOffset - 2); //subtract the cursor change from the menuOffset
    _cursorOffset = 2;
  } else if (_cursorOffset < 0) { //cursor is offscreen above
    _menuOffset -= calculateMenuOffset(_cursorOffset); //add the cursor change from the menuOffset
    _cursorOffset = 0; //set cursor to top row
  } //otherwise cursor must still be onscreen and offset has already been changed

  while (_menuOffset < 0) { //if offset is negative
    _menuOffset += _numberOfStates; //add states to wrap it
  }

  while (_menuOffset > _numberOfStates) { //if offset is positive over limit
    _menuOffset -= _numberOfStates; //remove states to wrap it
  }
}

int bikeMenu::calculateMenuOffset(int change) {

  int difference = change; //calculate offset considering displayHeight

  while (difference > _numberOfStates) { //while the offset it still greater than the number of states
    difference -= _numberOfStates; //subtract number of states
  }

  return difference;
}

