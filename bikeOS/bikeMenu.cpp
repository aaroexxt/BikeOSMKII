#include <Arduino.h>
#include "bigFont.h"
#include "bikeMenu.h"
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include "joystickHelper.h" //joystick library


bikeMenu::bikeMenu(String[] menuStates, int states) {
  _menuStates = menuStates;
  _states = states;
}

bikeMenu::init(LiquidCrystal_I2C & lcd, bigFont & customFont, DHT & dht, joystickHelper & joystick) : lcdRef (lcd) : fontRef (customFont) : dhtRef (dht) : joystickRef (joystick) {}
//Initializes all references to what's passed in







  



void renderMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MENU --------------");
  int counter = menuOffset;
  for (int i = 0; i < 3; i++) { //only paint 3 rows (lcd height 4)
    if (counter > numberOfStates) {
      counter = 0; //wrap it back around to print original states
    } else if (counter < 0) { //add number of states to wrap around (really shouldn't happen but in case it does)
      counter += numberOfStates;
    }
    lcd.setCursor(0, i+1); //must add 1 because row count starts from 1
    if (i == cursorOffset) { //is the current row the row with the cursor
      lcd.write(126); //print the right arrow
    }
    lcd.setCursor(1, i+1);
    lcd.print(menuStates[counter].substring(0, 19)); //substring it so as to make it not break the screen logic
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
void changeMenuPosition(int change) {
  cursorOffset += change;
  if (cursorOffset > 2) { //cursor is offscreen below
    menuOffset += calculateMenuOffset(cursorOffset - 2); //subtract the cursor change from the menuOffset
    cursorOffset = 2;
  } else if (cursorOffset < 0) { //cursor is offscreen above
    menuOffset -= calculateMenuOffset(cursorOffset); //add the cursor change from the menuOffset
    cursorOffset = 0; //set cursor to top row
  } //otherwise cursor must still be onscreen and offset has already been changed

  while (menuOffset < 0) { //if offset is negative
    menuOffset += numberOfStates; //add states to wrap it
  }

  while (menuOffset > numberOfStates) { //if offset is positive over limit
    menuOffset -= numberOfStates; //remove states to wrap it
  }
}

int calculateMenuOffset(int change) {

  int difference = change; //calculate offset considering displayHeight

  while (difference > numberOfStates) { //while the offset it still greater than the number of states
    difference -= numberOfStates; //subtract number of states
  }

  return difference;
}

// t is time in seconds = millis()/1000;
char * timeToString(unsigned long t)
{
  static char str[12];
  long h = t / 3600;
  t = t % 3600;
  int m = t / 60;
  int s = t % 60;
  sprintf(str, "%02ld:%02d:%02d", h, m, s);
  return str;
}