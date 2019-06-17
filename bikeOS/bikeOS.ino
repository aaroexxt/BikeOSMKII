//Library defs
#include <Arduino.h>
#include <ServoTimer2.h>
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h>

#include "pins.h" //pin definitions
#include "helper.h" //helper functions
#include "joystickHelper.h" //joystick library
/*struct joystickPosition {
  int x;
  int y;
};*/

//Adapted from ElectroNoobs ESC Controller by Aaron Becker
//EbikeOS by Aaron Becker. Let's get it
//Coded in May/Jun 2019, today is Jun 12. hopefully I'll make this super advanced at some point but rn it's pretty basic

int currentSpeed = 1200;
int currentPercent = 0;
int throttleValue = 0;
int oldPercent = 0;
int illegalMode = LOW;
int oldIllegalMode = HIGH;
boolean forceRedraw = false; //boolean to force a redraw of the display

//all sensor values
int reedVal;
long timeBetweenRevolution;// time between one full rotation (in ms)
long ridetime;
int mph;
int oldmph;
int maxReedTime = 2000; //maximum time between pulses
float circumference = (14.25*2*3.14159); //radius 14.25 (measured)
float odometer;
float oldodometer;

int maxReedCounter = 10; //min time (in ms) of one rotation (for debouncing)
int reedCounter;

//menu stuff
int menuOffset = 0;
int cursorOffset = 0; //cursor offset from top

unsigned long previousMillis = 0;
#define LED_ILLEGAL_MODE_INTERVAL 100
int ledState = LOW;

int MASTER_STATE = 0; //state machine

//Initialize libs
ServoTimer2 VESC; //Create VESC "servo" output
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 20 chars and 4 line display
joystickHelper joystick(joystickX, joystickY, joystickSW);

//create menu object
String menuStates[5] = {"Back", "Ride Time", "Temperature", "BETA CC Mode", "Info"};
int numberOfStates = 5;

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
                                           //That's why I map the values from 1505 to 2000u in order to avoid backwards rotations

  //setup lcd
  lcd.init(); //initialize lcd
  lcd.backlight(); //enable backlight
  lcd.home(); //clear lcd

  lcd.setCursor(0,0);
  lcd.print("Welcome!");
  lcd.setCursor(0,1);
  lcd.print("EBikeOS V1");
  lcd.setCursor(0,2);
  lcd.print("By Aaron Becker");
  lcd.setCursor(0,3);
  lcd.print("--------------------");

  // TIMER SETUP- the timer interrupt allows precise timed measurements of the reed switch
  //for more info about configuration of arduino timers see http://arduino.cc/playground/Code/Timer1
  cli();//stop interrupts

  //set timer1 interrupt at 1kHz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;
  // set timer count for 1khz increments
  OCR1A = 1999;// = (1/1000) / ((1/(16*10^6))*8) - 1
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler
  TCCR1B |= (1 << CS11);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts
  //END TIMER SETUP
  
  //Fix watchdog
  wdt_disable();
  delay(100);
}

ISR(TIMER1_COMPA_vect) {//Interrupt at freq of 1kHz to measure reed switch
  reedVal = digitalRead(reed_pin);//get val of A0
  if (reedVal == LOW) { //if reed switch is closed
    if (reedCounter <= 0) { //min time between pulses has passed
      mph = (56.8 * float(circumference)) / float(timeBetweenRevolution); //calculate miles per hour
      odometer += ((float(mph) / 60.0 / 60.0 / 1000.0) * float(timeBetweenRevolution)); //calculate miles moved and add to odometer; miles per milisecond*timer in ms
      timeBetweenRevolution = 0;//reset timer
      reedCounter = maxReedCounter; //reset reedCounter
    }
  }
  else { //if reed switch is open
    if (reedCounter > 0) { //don't let reedCounter go negative
      reedCounter -= 1;//decrement reedCounter
    }
  }
  if (timeBetweenRevolution > maxReedTime) {
    mph = 0;//if no new pulses from reed switch- tire is still, set mph, rpm and kph to 0
    ridetime -= maxReedTime;
  }
  else {
    timeBetweenRevolution += 1;//increment timer
    ridetime += 1;
  }
}

void loop() {
  //Serial.println(analogRead(joystickSW));
  switch (MASTER_STATE) {
    case 0: //initial state. Sets up LCD with status message
      lcd.clear();
      lcd.print("System OK.");
      lcd.setCursor(0,1);
      lcd.print("--------------------");
      lcd.setCursor(0,2);
      lcd.print("Press joystick to");
      lcd.setCursor(0,3);
      lcd.print("continue");
      
      MASTER_STATE = 1;
      break;
    case 1: //waits for joystick press
      if (joystick.isPressed() || true) {
        MASTER_STATE = 2; //boom switch states
      }
      break;
    case 2: //display basic LCD functions and then switch to state 3 (i.e. the main loop)
      lcd.clear();
      lcd.print("MPH:");
      lcd.setCursor(0,1);
      lcd.print("ODO:");
      lcd.setCursor(0,2);
      lcd.print("THT:");
      lcd.setCursor(0,3);
      lcd.print("IllegalMode:");
      
      forceRedraw = true; //force a redraw
      MASTER_STATE = 3;
      break;
    case 3: //do all the main checks. If there's menu desire, switch to state 4
      if (oldmph != mph || forceRedraw) {
        lcd.setCursor(0,0);
        lcd.print("                    ");
        lcd.setCursor(0,0);
        lcd.print("MPH: ");
        lcd.setCursor(5,0);
        lcd.print(mph);
      }
      if (oldodometer != odometer || forceRedraw) {
        lcd.setCursor(0,1);
        lcd.print("                    ");
        lcd.setCursor(0,1);
        lcd.print("ODO:");
        lcd.setCursor(5,1);
        lcd.print(String(odometer).substring(0,4));
      }
      if (oldIllegalMode != illegalMode || forceRedraw) {
          lcd.setCursor(0,3);
          lcd.print("                    ");
          lcd.setCursor(0,3);
          lcd.print("IllegalMode:");
          lcd.setCursor(13,3);
          lcd.print((illegalMode)?"ON":"OFF"); //ooo fancy ternary operator
      }
      if (oldPercent != currentPercent || forceRedraw) {
          lcd.setCursor(5,2);
          lcd.print(currentPercent);
          //todo finish this and make it a bar
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
      if (joystick.isPressed() && menuOffset == 0) {
        MASTER_STATE = 2; //go back
      }

      if (joystick.movement) {
        changeMenuPosition((joystick.down) ? 1 : -1); //change the position according to the magnitude of the joystick press
        renderMenu();
      }
      
      break;
    default: //hmm undefined state? so just reset
      Serial.println("State error: undefined state");
      Serial.println(MASTER_STATE);
      MASTER_STATE = 0;
      break;
  }

/*******
 * THE ESSENTIAL CODE
 * Controls all the throttle stuff. Must run every loop to keep bike responsive. Very important, for saftey especially!
 ******/
  throttleValue = analogRead(throt_pin);
  delay(50); //small delay so value isn't the same
  illegalMode = digitalRead(switch_pin);
  currentPercent = map(throttleValue, THROTTLE_MIN, THROTTLE_MAX, 0, 100);

  
  if (illegalMode == LOW) {
    //mult speed by constant or constriain: 2 approaches
    //speed = constrain(speed, 0, NON_ILLEGAL_SPEED_MAX);
    currentSpeed = map(throttleValue, THROTTLE_MIN, THROTTLE_MAX, ESC_MIN, ESC_LEGAL_MAX);    //map the 0 to 1024 ponts of analog read
    
    lightLedIfSpeed(HIGH); //light high
  } else {
    currentSpeed = map(throttleValue, THROTTLE_MIN, THROTTLE_MAX, ESC_MIN, ESC_MAX);    //map the 0 to 1024 ponts of analog read
    
    lightLedIfSpeed(ledState); //state according to millis loop
  }

  currentSpeed = constrain(currentSpeed, ESC_MIN, ESC_MAX);       //make sure the signal will never be over the range
  VESC.write(currentSpeed);

  if ((millis() - previousMillis) > LED_ILLEGAL_MODE_INTERVAL) { //toggle led state in background; will be activated during illegal mode
    previousMillis = millis();

    if (ledState == LOW) { //toggle ledState
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
  }
/*****
 * END ESSENTIAL CODE
 */
}


void lightLedIfSpeed(int _ledState) {
  if (currentSpeed > MIN_SPEED_LED) {
    digitalWrite(led_pin, _ledState); //enable if high
  } else {
    digitalWrite(led_pin, LOW); //disable if speed low
  }
}

void renderMenu() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("MENU --------------");
  int counter = menuOffset;
  for (int i=1; i<=3; i++) { //only paint 3 rows (lcd height 4)
    if (counter > numberOfStates) {
      counter = 0; //wrap it back around to print original states
    }
    lcd.setCursor(0,i);
    if (i-1 == cursorOffset) { //is the current row the row with the cursor
      lcd.write(126); //print the right arrow
    }
    lcd.setCursor(2,i);
    lcd.print(menuStates[counter]);
    counter++;
  }
}

void changeMenuPosition(int change) {
  cursorOffset += change;
  if (cursorOffset > 3) { //cursor is offscreen below
    cursorOffset = 3; //set cursor to bottom row
    menuOffset = calculateMenuOffset(change-3); //subtract the cursor change from the menuOffset
  } else if (cursorOffset < 0) { //cursor is offscreen above
    cursorOffset = 0; //set cursor to top row
    menuOffset = calculateMenuOffset(change+3); //add the cursor change from the menuOffset
  } //otherwise cursor must still be onscreen and offset has already been changed
}

int calculateMenuOffset(int change) {
  
  int difference = change - 3; //calculate offset considering displayHeight

  while(difference > numberOfStates) { //while the offset it still greater than the number of states
    difference -= numberOfStates; //subtract number of states
  }

  return -difference;
}

