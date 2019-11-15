/***************
  ______     __     __  __     ______     ______     ______        __    __     __  __     __     __
  /\  == \   /\ \   /\ \/ /    /\  ___\   /\  __ \   /\  ___\      /\ "-./  \   /\ \/ /    /\ \   /\ \
  \ \  __<   \ \ \  \ \  _"-.  \ \  __\   \ \ \/\ \  \ \___  \     \ \ \-./\ \  \ \  _"-.  \ \ \  \ \ \
  \ \_____\  \ \_\  \ \_\ \_\  \ \_____\  \ \_____\  \/\_____\     \ \_\ \ \_\  \ \_\ \_\  \ \_\  \ \_\
  \/_____/   \/_/   \/_/\/_/   \/_____/   \/_____/   \/_____/      \/_/  \/_/   \/_/\/_/   \/_/   \/_/

****************


  Heavily adapted from ElectroNoobs ESC Controller by Aaron Becker
  EbikeOS by Aaron Becker. Let's get it
  V1 May/Jun 2019, V2 Oct/Nov 2019
*/

//Library defs
#include <Arduino.h>
#include <ServoTimer2.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <avr/wdt.h>

#include "pins.h" //pin definitions
#include "helper.h" //helper functions
#include "joystickHelper.h" //joystick library
#include "bigFont.h" //big font library
#include "bikeMenu.h" //menu library
#include "MemoryFree.h" //Free memory

//Current tracking values
int currentSpeed = 1200; //in PPM
int currentPercent = 0;
int throttleValue = 0; //in percent

int illegalMode = LOW;
boolean forceRedraw = false; //boolean to force a redraw of the display

//All 'old' variables - to keep track of changes between loop cycles
//OLDVARS-Control
int oldPercent = 0;
int oldIllegalMode = HIGH;

//OLDVARS-Sensors
int oldMph;
float oldF, oldH, oldHi;
float oldOdometer;


//Sensor values
//SENS-RIDING
int reedVal;
int maxReedTime = 2000; //maximum time between pulses
long timeBetweenRevolution;// time between one full rotation (in ms)
boolean canDecrementRidetime = true; //only remove time from rideTime if we're actually moving

int maxReedCounter = 10; //min time (in ms) of one rotation (for debouncing)
int reedCounter; //ms since last rotation


//The important variables
float odometer;
int mph;
long ridetime = 0; //time spent with mph>0


//Constants
float WHEEL_CIRCUMFERENCE = (14.5 * 2 * 3.14159); //radius 14.5? TODO TUNE THIS
#define LED_ILLEGAL_MODE_INTERVAL 100
#define LED_CC_MODE_INTERVAL 250

unsigned long illegalModePrevTime = 0;

int ledState = LOW;

int MASTER_STATE = 0; //state machine

#define LCD_MENU_STYLE 2 //menu style 2

//Initialize libs

//SETUP LCD
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 20 chars and 4 line display

//SETUP VESC
ServoTimer2 VESC; //Create VESC "servo" output

//SETUP LIBRARIES
joystickHelper joystick(joystickX, joystickY, joystickSW);
DHT dht(temp_pin, tempType); //temperature sensor object
bigFont *font;
bikeMenu *menu;

//SETUP MENU CONSTANTS
String menuStates[5] = {"Back", "Ride Time", "Temperature", "BETA CC Mode", "Info"};
const int numberOfMenuStates = 5;

//TIMER STUFF
typedef void (*timerFuncPtr)(void); // Create a type to point to a funciton.
struct timerElement { //Struct that contains a function pointer and a time to trigger it i.e. how often should we trigger?
  timerFuncPtr func;
  int time;
};

//TIMER FUNCTIONS
void memCheck() {
  if (freeMemory() < 100) { //we're running out of memory space
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Memory critical");
    lcd.setCursor(0,1);
    lcd.print("Arduino restarting");
    while(1){} //hang until watchdog resets processor
  }
}

//TIMER ARRAY INITIALIZATION
timerElement timedEvents[5] = {{memCheck, 30000}}; //create the timer
const int timerElements = 1;

long prevMillis[timerElements]; //auto sets to 0


//CC STUFF
boolean CCEnabled = false;
float CCSetpoint = 10.0;
float CCTargetPwr = 0;
//Todo: determine if delta is converging to 0, to ensure that we're actually updating speeds
// float CCDelta[10] = new float[10];

void setup() {
  Serial.begin(57600);
  //setup pins
  pinMode(switch_pin, INPUT_PULLUP);
  pinMode(reed_pin, INPUT_PULLUP);
  pinMode(vesc_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);

  //setup vesc
  VESC.attach(vesc_pin);
  VESC.write(ESC_MIN);           //The range of this ESC is 1000us to 1500 for CW and 1500 to 2000us for CCW
  //That's why I map the values from 1505 to 2000u in order to avoid backwards rotations (also requires configuring VESC in its own software)
  //setup temp
  dht.begin();

  //setup lcd
  lcd.init(); //initialize lcd
  lcd.backlight(); //enable backlight
  lcd.home(); //clear lcd

  //Reset all LCD references because it's now initialized
  bigFont pFont(lcd); //big font object (pass in lcd)
  bikeMenu pMenu(menuStates, numberOfMenuStates, lcd); //pass in everything
  font = &pFont;
  menu = &pMenu;

  // TIMER SETUP- the timer interrupt allows precise timed measurements of the reed switch
  //for more info about configuration of arduino timers see http://arduino.cc/playground/Code/Timer1
  cli();//stop interrupts
  TCCR1B |= (1 << CS11);


  //set timer1 interrupt at 1kHz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;
  // set timer count for 1khz increments
  OCR1A = 1999;// = (1/1000) / ((1/(16*10^6))*8) - 1
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts
  //END TIMER SETUP

  //Fix watchdog
  wdt_enable(WDTO_2S);
  wdt_reset();
  delay(100);
}

ISR(TIMER1_COMPA_vect) {//Interrupt at freq of 1kHz to measure reed switch
  reedVal = digitalRead(reed_pin);//get val of A0
  if (reedVal == LOW) { //if reed switch is closed
    if (reedCounter <= 0) { //min time between pulses has passed
      mph = (56.8 * float(WHEEL_CIRCUMFERENCE)) / float(timeBetweenRevolution); //calculate miles per hour
      odometer += ((float(mph) / 60.0 / 60.0 / 1000.0) * float(timeBetweenRevolution)); //calculate miles moved and add to odometer; miles per milisecond*timer in ms
      timeBetweenRevolution = 0;//reset timer
      reedCounter = maxReedCounter; //reset reedCounter
    }
    canDecrementRidetime = true;
  }
  else { //if reed switch is open
    if (reedCounter > 0) { //don't let reedCounter go negative
      reedCounter -= 1;//decrement reedCounter
    }
  }
  if (timeBetweenRevolution > maxReedTime) {
    mph = 0;//if no new pulses from reed switch- tire is still, set mph, rpm and kph to 0
    if (ridetime > maxReedTime && canDecrementRidetime) { //allow only one subtraction
      ridetime -= maxReedTime;
      canDecrementRidetime = false;
    }
  } else {
    timeBetweenRevolution += 1;//increment timer
    ridetime += 1;
  }
}

void loop() {
  wdt_reset(); //pat watchdog (in case arduino gets stuck in a loop)

/* Master-state breakdown

0 - initial, writes hello message
1 - delay of hello message
2 - print V1 lcd message once
3 - update loop for 'V1 style' lcd
4 - print V2 lcd message once
5 - update loop for 'V2 style' lcd
6 - menu
7 - menu

*/
  switch (MASTER_STATE) {
    case 0: //initial state. Sets up LCD with status message
      lcd.setCursor(0, 0);
      font->writeString("BKOS2", 0, 0);
      lcd.setCursor(0,3);
      lcd.print("By Aaron Becker");
      delay(1500); //small delay

      transitionState(LCD_MENU_STYLE, true); //switch state
      break;
    case 1: //V1 style lcd update loop
      if (oldMph != mph || forceRedraw) {
        lcd.setCursor(0, 0);
        lcd.print("                   ");
        lcd.setCursor(0, 0);
        lcd.print("MPH: ");
        lcd.setCursor(5, 0);
        lcd.print(mph);
      }
      if (oldOdometer != odometer || forceRedraw) {
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
        transitionState(6);
      }

      //reset various "old" variables
      oldMph = mph;
      oldOdometer = odometer;
      oldIllegalMode = illegalMode;
      oldPercent = currentPercent;
      break;
    case 2: //V2 style lcd update loop

      //Run all the checks
      if (oldIllegalMode != illegalMode || forceRedraw) {
        lcd.setCursor(17,3);
        lcd.print((illegalMode)?"EN":"DS");
      }

      if (oldPercent != currentPercent || forceRedraw) {
        lcd.setCursor(15,4);
        lcd.print(currentPercent);
        lcd.setCursor(19,4);
        lcd.print("%");
      }

      if (oldMph != mph || forceRedraw) {
        lcd.setCursor(16,0);
        lcd.print(String(mph).substring(0, 4));
      }

      if (oldOdometer != odometer || forceRedraw) {
        lcd.setCursor(16,2);
        lcd.print(String(odometer).substring(0, 4));
      }

      if (joystick.isPressed()) { //update joystick state in this mode so as to not waste processing power otherwise
        transitionState(6);
      }

      //reset various "old" variables
      oldMph = mph;
      oldOdometer = odometer;
      oldIllegalMode = illegalMode;
      oldPercent = currentPercent;
    case 3: //menu on screen; case to wait until joystick change and update x value
      delay(100); //don't update too quickly
      joystick.update();
      if (joystick.isPressed()) { //if joystick pressed
        int offset = menu->getOffset();
        if (offset != 0) { //offset is 0, so just return otherwise switch state
          transitionState(offset + 4, true); //force a screen redraw to paint initial state
          return; //return because we don't want the state to be updated anymore
        } else {
          transitionState(LCD_MENU_STYLE, true);
        }
      }

      if (joystick.movement) { //is joystick moved? change menu state
        menu->changeMenuPosition((joystick.down) ? 1 : -1); //change the position according to the magnitude of the joystick press
        menu->renderMenu();
      }

      break;
    case 4: //ridetime
      delay(100);
      lcd.setCursor(0, 1);
      lcd.print(timeToString(ridetime / 1000)); //prints time riding
      Serial.println(ridetime);
      lcd.setCursor(0, 3);
      lcd.print(timeToString(millis() / 1000)); //prints time since init

      if (joystick.isPressed()) {
        transitionState(LCD_MENU_STYLE,true);
      }
      break;
    case 5: //temp
      delay(100);

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
        if (oldF != f || forceRedraw) {
          lcd.setCursor(0, 1);
          lcd.print("T: ");
          lcd.print(f);
        }
        if (oldH != h || forceRedraw) {
          lcd.setCursor(0, 2);
          lcd.print("H: ");
          lcd.print(h);
        }
        if (oldHi != hi || forceRedraw) {
          lcd.setCursor(0, 3);
          lcd.print("HI: ");
          lcd.print(hi);
        }
        oldHi = hi; //reset oldHi
      } else {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Error reading");
        delay(1000);
        MASTER_STATE = 2;
      }
      //reset old storage vars
      oldF = f;
      oldH = h;

      if (joystick.isPressed()) {
        transitionState(LCD_MENU_STYLE,true);
      }
      break;
    case 6: //beta cc mode
      delay(20);
      joystick.update(); //update joystick (poll)
      
      if (joystick.isPressed()) {
        transitionState(LCD_MENU_STYLE,true);
      } else if (joystick.movement) { //if joystick is getting moved, change the setpoint
        CCSetpoint += ((joystick.down) ? -0.5 : 0.5);

        font->writeString(String(CCSetpoint).substring(0, 4), 0, 1); //also update the setpoint value
      }

      if (oldMph != mph || forceRedraw) {
        lcd.setCursor(0,3);
        lcd.print("T: 100.0% E: 000.0");
        lcd.setCursor(10, 3);
        lcd.write(130); //print the delta sign

        lcd.setCursor(3, 3);
        int mappedPwr = mapF(CCTargetPwr, CC_MIN, CC_MAX, 0, 100);
        lcd.print(String(mappedPwr).substring(0,4)); //print target power

        lcd.setCursor(13, 3);
        lcd.print(String(mph-CCSetpoint).substring(0,4)); //print delta mph
      }

      break;
    case 7: //info
      delay(100);
      if (joystick.isPressed()) {
        transitionState(LCD_MENU_STYLE,true);
      }
      break;

    default: //hmm undefined state? so just reset
      Serial.println("State error: undefined state");
      Serial.println(MASTER_STATE);
      transitionState(0);
      break;
  }
  
  //Event checking
  unsigned long currentMillis = millis();
  for (int i=0; i<timerElements-1; i++) {
    if (currentMillis-prevMillis[i] >= timedEvents[i].time) {
      prevMillis[i] = currentMillis;
      timedEvents[i].func(); //call function
    }
  }

  //The bike code that does the driving
  if (CCEnabled) {
    driveUpdateCC();
  } else {
    driveUpdateManual(); //update the bike motor power 
  }

  //Disable forced redraw
  forceRedraw = false; //disable forced redraw
  
}

void driveUpdateManual() { //the big boi code that controls whether to update drive
  /*******
     THE ESSENTIAL CODE
     Controls all the throttle stuff. Must run every loop to keep bike responsive. Very important, for saftey especially!
   ******/
  throttleValue = analogRead(throt_pin);
  delay(50); //small delay so value isn't the same
  illegalMode = digitalRead(switch_pin);
  currentPercent = map(throttleValue, THROTTLE_MIN, THROTTLE_MAX, 0, 100);


  if (illegalMode == HIGH) {
    //mult speed by constant or constrain: 2 approaches
    //speed = constrain(speed, 0, NON_ILLEGAL_SPEED_MAX);
    currentSpeed = map(throttleValue, THROTTLE_MIN, THROTTLE_MAX, ESC_MIN, ESC_LEGAL_MAX);    //map the 0 to 1024 ponts of analog read

    lightLedIfSpeed(HIGH); //light high
  } else {
    currentSpeed = map(throttleValue, THROTTLE_MIN, THROTTLE_MAX, ESC_MIN, ESC_MAX);    //map the 0 to 1024 ponts of analog read

    lightLedIfSpeed(ledState); //state according to millis loop
  }

  currentSpeed = constrain(currentSpeed, ESC_MIN, ESC_MAX);       //make sure the signal will never be over the range
  VESC.write(currentSpeed);

  if ((millis() - illegalModePrevTime) > LED_ILLEGAL_MODE_INTERVAL) { //toggle led state in background; will be activated during illegal mode
    illegalModePrevTime = millis();

    if (ledState == LOW) { //toggle ledState
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
  }
  /*****
     END ESSENTIAL CODE
  */
}

void driveUpdateCC() { //saucy very simple cc code
  float delta = mph-CCSetpoint;

  if (delta > 0) {
    CCTargetPwr -= 0.005;
  } else if (delta > 0) {
    CCTargetPwr += 0.001;
  }
  CCTargetPwr = constrain(CCTargetPwr, CC_MIN, CC_MAX);

  currentPercent = mapF(CCTargetPwr, CC_MIN, CC_MAX, 0, 100);
  currentSpeed = mapF(CCTargetPwr, CC_MIN, CC_MAX, ESC_MIN, ESC_MAX);    //map the 0 to 1024 ponts of analog read
  lightLedIfSpeed(ledState); //state according to millis loop

  currentSpeed = constrain(currentSpeed, ESC_MIN, ESC_MAX);       //make sure the signal will never be over the range
  VESC.write(currentSpeed);

  if ((millis() - illegalModePrevTime) > LED_CC_MODE_INTERVAL) { //toggle led state in background; will be activated during illegal mode
    illegalModePrevTime = millis();

    if (ledState == LOW) { //toggle ledState
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
  }
}

void transitionStateReal(int newState, boolean forceRD) {
  forceRedraw = forceRD;
  MASTER_STATE = newState;
  CCEnabled = false; //default it

  switch (newState) { //do something when transitioning states
    case 1: //V1 style menu initial paint
      lcd.clear();
      lcd.print("MPH:");
      lcd.setCursor(0, 1);
      lcd.print("ODO:");
      lcd.setCursor(0, 2);
      lcd.print("THT:");
      lcd.setCursor(0, 3);
      lcd.print("IllegalMode:");
      break;
    case 2: //V2 style menu initial paint
      lcd.clear();
      font->writeString("MPH:", 0, 0);
      font->writeString("ODO:", 0, 2);
      lcd.setCursor(15,3);
      lcd.print("I:EN");
      lcd.setCursor(15, 4);
      lcd.print("100%");
      break;
    case 3: //Menu initial paint
      menu->renderMenu(); //render menu onto LCD
      delay(100);
      break;
    case 4: //ridetime
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ride Time ----------");
      lcd.setCursor(0, 2);
      lcd.print("Time Since Init");
      break;
    case 5: //temp
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp ----------------");
      lcd.setCursor(0, 1);
      lcd.print("T: ");
      lcd.setCursor(0, 2);
      lcd.print("H: ");
      lcd.setCursor(0, 3);
      lcd.print("HI: ");
      break;
    case 6: //cc setpoint
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CC SETPOINT MODE");
      font->writeString("10.5", 0, 1);
      lcd.setCursor(16,2);
      lcd.print("mph");
      lcd.setCursor(0,3);
      lcd.print("T: 100.0% E: 000.0%");

      //ENABLE CC
      CCEnabled = true;
      break;
    case 7: //info
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Info --------------");
      lcd.setCursor(0, 1);
      lcd.print("BikeOS MKII");
      lcd.setCursor(0, 2);
      lcd.print("By Aaron Becker");
      lcd.setCursor(0, 3);
      lcd.print("Copyright 2019");
      break;

  }

}

//Overloaded constructors
void transitionState(int newState) {
  transitionStateReal(newState, false);
}

void transitionState(int newState, boolean forceRD) {
  transitionStateReal(newState, forceRD);
}


void lightLedIfSpeed(int _ledState) { //Function to check if throttle value is high enough to merit lighting lcd
  if (currentSpeed > MIN_SPEED_LED) {
    digitalWrite(led_pin, _ledState); //enable if high
  } else {
    digitalWrite(led_pin, LOW); //disable if speed low
  }
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
