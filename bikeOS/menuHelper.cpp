//menu helper library
#include <Arduino.h>
#include "menuHelper.h"
#include <LiquidCrystal_I2C.h>
#include <stdio.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

LiquidCrystal_I2C *o;

menuHelper::menuHelper(LiquidCrystal_I2C *_LCDreference, String _menuStates[], int _numberOfStates) {
  menuHelper::LCDreference = _LCDreference; //set correct type
  o = _LCDreference;
  
  menuHelper::menuOffset = 0;
  menuHelper::cursorOffset = 0;

  menuHelper::numberOfStates = _numberOfStates;
  *menuHelper::menuStates = new String[_numberOfStates]; //use pointer to reference array
  memcpy(menuHelper::menuStates, _menuStates, sizeof(_menuStates[0]) * _numberOfStates); //memcpy the values from the array into the correctly sized menuStates
}

int menuHelper::getMenuOffset() { //so this function has to take into account both cursor and menu offsets and return a master offset
  int offset = menuHelper::menuOffset+menuHelper::cursorOffset;
  return offset;
}

void menuHelper::renderMenu() {

  o->clear();
  o->setCursor(0,0);
  o->print("MENU --------------");
  int counter = menuHelper::menuOffset;
  for (int i=1; i<(menuHelper::displayHeight-2); i++) { //only paint 3 rows
    o->setCursor(0,i);
    if (i-1 == menuHelper::cursorOffset) { //is the current row the row with the cursor
      o->write(126); //print the right arrow
    }
    o->print(*menuHelper::menuStates[counter]);
    
    counter++;
    if (counter > menuHelper::numberOfStates) {
      counter = 0; //wrap it back around to print original states
    }
  }
}

//TODO handle edge case where menuStates is less than displayHeight-1, because bottom row code will fail
void menuHelper::changeMenuPosition(int change) {
  menuHelper::cursorOffset += change;
  if (menuHelper::cursorOffset > (menuHelper::displayHeight-1)) { //cursor is offscreen below
    menuHelper::cursorOffset = (menuHelper::displayHeight-1); //set cursor to bottom row
    menuHelper::menuOffset = calculateMenuOffset(change-(menuHelper::displayHeight-1)); //subtract the cursor change from the menuOffset
  } else if (menuHelper::cursorOffset < 0) { //cursor is offscreen above
    menuHelper::cursorOffset = 0; //set cursor to top row
    menuHelper::menuOffset = calculateMenuOffset(change+(menuHelper::displayHeight-1)); //add the cursor change from the menuOffset
  } //otherwise cursor must still be onscreen and offset has already been changed
  
}

int menuHelper::calculateMenuOffset(int change) {
  
  int difference =  change - (menuHelper::displayHeight-1); //calculate offset considering displayHeight

  while(difference > menuHelper::numberOfStates) { //while the offset it still greater than the number of states
    difference -= menuHelper::numberOfStates; //subtract number of states
  }
}

