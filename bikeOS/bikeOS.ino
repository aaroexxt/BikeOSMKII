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
int oldmph;
float oldf, oldh, oldhi;
float oldodometer;


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

//Menu Offsetting
int menuOffset = 0;
int cursorOffset = 0; //cursor offset from top

unsigned long illegalModePrevTime = 0;

int ledState = LOW;

int MASTER_STATE = 0; //state machine

/*** THINGS TO IMPLEMENT
APP_STATE vs MENU_STATE
App state - i.e. init, control
Everything packaged in classes

menu its own class/library - decentralized from this file.
called with menu.setMenuState, menu.getMenuState, etc like java
menu.renderReturn returns array with 4 strings - output for display

joystick its own class - reuse from old time
swipe right/left to view sensor data. up/down on main screen to change the mode from PID to manual control
PID like subaru control - instantly exits if anything is pressed (throttle change position, joystick clicked/moved, etc). also should always run in "legal" mode (ignores position of IllegalMode switch)

MAIN SCREEN DISPLAY:
Mode: Manual
Throttle: X%
IllegalMode: Enabled/Disabled
MPH: mph mi, ODO: odo mi

PID screen display:
Mode: SemiAuto
Setpoint: 10mph
Click to inc/dec SP
VESC actually controls PID of motor, but controller should linearly increase w/feedback from speed sensor. if no feedback, error out

BIGMPH DISPLAY:
mph in big font
at bottom, odo




*/

//Initialize libs

//SETUP LCD
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 20 chars and 4 line display

//SETUP VESC
ServoTimer2 VESC; //Create VESC "servo" output

//SETUP LIBRARIES
joystickHelper joystick(joystickX, joystickY, joystickSW);
DHT dht(temp_pin, tempType); //temperature sensor object
bigFont bigFont(lcd); //big font object (pass in lcd)

//SETUP MENU LIBRARIES

String menuStates[5] = {"Back", "Ride Time", "Temperature", "BETA CC Mode", "Info"};
int numberOfStates = 5;
BikeMenu menu(menuStates, lcd, bigFont, dht, joystick); //pass in everything


//TIMER STUFF
typedef void (*timerFuncPtr)(void); // Create a type to point to a funciton.
struct timerElement { //Struct that contains a function pointer and a time to trigger it i.e. how often should we trigger?
  timerFuncPtr func;
  int time;
};

timerElement timedEvents[5] = {{memCheck, 30000}, {menu.update, 200}}; //create the timer
const int timerElements = 5;

long prevMillis[timerElements]; //auto sets to 0

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
  bigFont.init(lcd);
  bikeMenu.init(lcd, bigFont, dht, joystick);
  bikeMenu.transitionState(0);

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
      bigFont.writeString("BKOS2", 0, 0);
      lcd.setCursor(0,3);
      lcd.print("By Aaron Becker");

      MASTER_STATE = 1;
      break;
    case 1: //waits for time
      delay(1000);
      MASTER_STATE = 2; //boom switch states
      break;
    case 2: //V1 style LCD functions and then switch to state 3 (i.e. the main loop)
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
    case 3: //do all the main checks. If there's menu desire, switch to state 6
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

    case 4:
      lcd.clear();
      bigFont.writeString("MPH: ", 0, 0);
      bigFont.writeString("ODO: ", 0, 2);
      lcd.setCursor(16, 4);
      lcd.print("100%");
      lcd.setCursor(16,3);
      lcd.print("I:EN");

    case 6: //paint menu screen
      renderMenu(); //render menu onto LCD
      delay(500);
      MASTER_STATE = 5;
      break;
    case 7: //case to wait until joystick change and update x value
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
    case 8: //ridetime
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
    case 9: //temp
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
    case 10: //beta cc mode
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
    case 11: //info
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
  
  //Event checking
  unsigned long currentMillis = millis();
  for (int i=0; i<timerElements-1; i++) {
    if (currentMillis-prevMillis[i] >= timedEvents[i].time) {
      prevMillis[i] = currentMillis;
      timedEvents[i].func(); //call function
    }
  }

  //The bike code that does the driving
  /*if (pidDrive.enabled) {
    pidDrive.driveUpdate();
  } else {
    driveUpdateManual(); //update the bike motor power 
  }*/
  driveUpdateManual();
  
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
void lightLedIfSpeed(int _ledState) { //Function to check if throttle value is high enough to merit lighting lcd
  if (currentSpeed > MIN_SPEED_LED) {
    digitalWrite(led_pin, _ledState); //enable if high
  } else {
    digitalWrite(led_pin, LOW); //disable if speed low
  }
}

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
