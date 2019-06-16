//Adapted from ElectroNoobs ESC Controller by Aaron Becker

//TODO ADD LED
#include <Servo.h>                        //We need this library for the PWm signal
Servo VESC;                              //We create the servo pin, later attach to D3

#define pot_pin A6
#define switch_pin 3
#define vesc_pin 5
#define led_pin 13 //maybe I'll actually add an indicator led someday

#define ESC_MIN 1200
#define ESC_LEGAL_MAX 2000
#define ESC_MAX 2200
//#define ESC_MIDDLE=(ESC_MAX+ESC_MIN)/2

#define THROTTLE_MIN 190
#define THROTTLE_MAX 861
#define MIN_SPEED_LED ESC_MIN+100

int currentSpeed = 1200;
int throttleValue = 0;
int illegalMode = LOW;

unsigned long previousMillis = 0;
#define LED_ILLEGAL_MODE_INTERVAL 100
int ledState = LOW;

void setup() {
  pinMode(switch_pin, INPUT);
  pinMode(vesc_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  
  VESC.attach(vesc_pin);
  VESC.writeMicroseconds(ESC_MIN);           //The range of this ESC is 1000us to 1500 for CW and 1500 to 2000us for CCW
                                           //That's why I map the values from 1505 to 2000u in order to avoid backwards rotations
  delay(1000);  
}

void loop() {
  throttleValue = analogRead(pot_pin);
  illegalMode = digitalRead(switch_pin);

  
  if (illegalMode == LOW) {
    //mult speed by constant or constriain: 2 approaches
    //speed = constrain(speed, 0, NON_ILLEGAL_SPEED_MAX);
    currentSpeed = map(analogRead(pot_pin), THROTTLE_MIN, THROTTLE_MAX, ESC_MIN, ESC_LEGAL_MAX);    //map the 0 to 1024 ponts of analog read to our ranfe from 1505 to 2000us
    
    lightLedIfSpeed(HIGH); //light high
  } else {
    currentSpeed = map(analogRead(pot_pin), THROTTLE_MIN, THROTTLE_MAX, ESC_MIN, ESC_MAX);    //map the 0 to 1024 ponts of analog read to our ranfe from 1505 to 2000us
    
    lightLedIfSpeed(ledState); //state according to millis loop
  }

  currentSpeed = constrain(currentSpeed,ESC_MIN, ESC_MAX);       //make sure the signal will never be over the range
  VESC.writeMicroseconds(currentSpeed);

  if ((millis() - previousMillis) > LED_ILLEGAL_MODE_INTERVAL) { //toggle led state in background; will be activated during illegal mode
    previousMillis = millis();

    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
  }

}

void lightLedIfSpeed(int _ledState) {
  if (currentSpeed > MIN_SPEED_LED) {
    digitalWrite(led_pin, _ledState); //enable if high
  } else {
    digitalWrite(led_pin, LOW); //disable if speed low
  }
}

