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







  lcd.setCursor(0, 0);
  bigFont.writeString("BKOS2", 0, 0);
  lcd.setCursor(0,3);
  lcd.print("By Aaron Becker");

switch (MASTER_STATE) {
    case 0: //initial state. Sets up LCD with status message
      lcd.clear();
      lcd.print("System OK.");
      lcd.setCursor(0, 1);
      lcd.print("--------------------");
      lcd.setCursor(0, 2);
      lcd.print("Press joystick to");
      lcd.setCursor(0, 3);
      lcd.print("continue");

      MASTER_STATE = 1;
      break;
    case 1: //waits for joystick press
      if (joystick.isPressed()) {
        MASTER_STATE = 2; //boom switch states
      }
      break;
    case 2: //display basic LCD functions and then switch to state 3 (i.e. the main loop)
      lcd.clear();
      lcd.print("MPH:");
      lcd.setCursor(0, 1);
      lcd.print("ODO:");
      lcd.setCursor(0, 2);
      lcd.print("THT:");
      lcd.setCursor(0, 3);
      lcd.print("IllegalMode:");

      forceRedraw = true; //force a redraw
      MASTER_STATE = 3;
      break;
    case 3: //do all the main checks. If there's menu desire, switch to state 4
      if (oldmph != mph || forceRedraw) {
        lcd.setCursor(0, 0);
        lcd.print("                   ");
        lcd.setCursor(0, 0);
        lcd.print("MPH: ");
        lcd.setCursor(5, 0);
        lcd.print(mph);
      }
      if (oldodometer != odometer || forceRedraw) {
        lcd.setCursor(0, 1);
        lcd.print("                   ");
        lcd.setCursor(0, 1);
        lcd.print("ODO:");
        lcd.setCursor(5, 1);
        lcd.print(String(odometer).substring(0, 4));
      }
      if (oldIllegalMode != illegalMode || forceRedraw) {
        lcd.setCursor(0, 3);
        lcd.print("                   ");
        lcd.setCursor(0, 3);
        lcd.print("MaxPwrMode:");
        lcd.setCursor(12, 3);
        lcd.print((illegalMode) ? "Disabled" : "Enabled "); //ooo fancy ternary operator
      }
      if (oldPercent != currentPercent || forceRedraw) {
        lcd.setCursor(0, 2);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("THT: ");
        lcd.setCursor(5, 2);
        String throtBar = "";
        int barSegments = map(currentPercent, 0, 100, 0, 14);
        if (barSegments > 0) {
          for (int i = 0; i <= barSegments; i++) {
            throtBar += "-";
          }
        }
        throtBar += ">"; //close bar

        lcd.print(throtBar);
      }


      if (joystick.isPressed()) { //update joystick state in this mode so as to not waste processing power otherwise
        MASTER_STATE = 4;
      }

      //reset various "old" variables
      forceRedraw = false;
      oldmph = mph;
      oldodometer = odometer;
      oldIllegalMode = illegalMode;
      break;
    case 4: //paint menu screen
      renderMenu(); //render menu onto LCD
      delay(500);
      MASTER_STATE = 5;
      break;
    case 5: //case to wait until joystick change and update x value
      delay(100);
      joystick.update();
      if (joystick.isPressed()) { //if joystick pressed
        int offset = menuOffset + cursorOffset;
        if (offset != 0) { //offset is 0, so just return otherwise switch state
          MASTER_STATE = offset + 5; //force a screen redraw to paint initial state
          forceRedraw = true;
          return; //return because we don't want the state to be updated anymore
        } else {
          MASTER_STATE = 2;
        }
      }

      if (joystick.movement) {
        changeMenuPosition((joystick.down) ? 1 : -1); //change the position according to the magnitude of the joystick press
        renderMenu();
        forceRedraw = false;
      }

      break;
    case 6: //ridetime
      delay(100);
      if (forceRedraw) { //first paint
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Ride Time ----------");
        lcd.setCursor(0, 2);
        lcd.print("Time Since Init");
        forceRedraw = false; //disable forced redraw
      } else {
        lcd.setCursor(0, 1);
        lcd.print(timeToString(ridetime / 1000)); //prints time riding
        Serial.println(ridetime);
        lcd.setCursor(0, 3);
        lcd.print(timeToString(millis() / 1000)); //prints time since init
      }
      if (joystick.isPressed()) {
        MASTER_STATE = 2;
      }
      break;
    case 7: //temp
      delay(100);
      if (forceRedraw) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Temp ----------------");
        lcd.setCursor(0, 1);
        lcd.print("T: ");
        lcd.setCursor(0, 2);
        lcd.print("H: ");
        lcd.setCursor(0, 3);
        lcd.print("HI: ");
        forceRedraw = false; //disable forced redraw
      }

      //temp reading
      float h = dht.readHumidity();
      float f = dht.readTemperature(true);
      if (isnan(h)) {
        h = -1.0;
      }
      if (isnan(f)) {
        f = -1.0;
      }
      if (f > -1 && h > -1) {
        float hi = dht.computeHeatIndex(f, h);
        if (oldf != f || forceRedraw) {
          lcd.setCursor(0, 1);
          lcd.print("T: ");
          lcd.print(f);
        }
        if (oldh != h || forceRedraw) {
          lcd.setCursor(0, 2);
          lcd.print("H: ");
          lcd.print(h);
        }
        if (oldhi != hi || forceRedraw) {
          lcd.setCursor(0, 3);
          lcd.print("HI: ");
          lcd.print(hi);
        }
        oldhi = hi; //reset oldhi
      } else {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Error reading");
        delay(1000);
        MASTER_STATE = 2;
      }
      //reset old storage vars
      oldf = f;
      oldh = h;


      if (joystick.isPressed()) {
        MASTER_STATE = 2;
      }
      break;
    case 8: //beta cc mode
      delay(100);
      if (forceRedraw) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("not programmed press joystick to exit");
        forceRedraw = false; //disable forced redraw
      }
      if (joystick.isPressed()) {
        MASTER_STATE = 2;
      }
      break;
    case 9: //info
      if (forceRedraw) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Info --------------");
        lcd.setCursor(0, 1);
        lcd.print("BikeOS MKII");
        lcd.setCursor(0, 2);
        lcd.print("By Aaron Becker");
        lcd.setCursor(0, 3);
        lcd.print("Copyright 2019");
        forceRedraw = false; //disable forced redraw
      }
      if (joystick.isPressed()) {
        MASTER_STATE = 2;
      }
      break;

    default: //hmm undefined state? so just reset
      Serial.println("State error: undefined state");
      Serial.println(MASTER_STATE);
      MASTER_STATE = 0;
      break;
  }


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
    menuOffset = calculateMenuOffset(cursorOffset - 2); //subtract the cursor change from the menuOffset
    cursorOffset = 2;
  } else if (cursorOffset < 0) { //cursor is offscreen above
    menuOffset = calculateMenuOffset(cursorOffset); //add the cursor change from the menuOffset
    cursorOffset = 0; //set cursor to top row
  } //otherwise cursor must still be onscreen and offset has already been changed

  while (menuOffset < 0) { //if offset is negative
    menuOffset += numberOfStates; //add states to wrap it
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