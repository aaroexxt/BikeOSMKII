//Pin definitions
#define throt_pin A3
#define switch_pin 3
#define vesc_pin 5
#define led_pin 13
#define reed_pin A0
#define joystickX A2
#define joystickY A1
#define joystickSW 7
#define temp_pin A1 //pin to dht11 temp
#define tempType DHT22 //temperature sensor type

#define ESC_MIN 1200
#define ESC_LEGAL_MAX 2000
#define ESC_MAX 2200
//#define ESC_MIDDLE=(ESC_MAX+ESC_MIN)/2

#define THROTTLE_MIN 190
#define THROTTLE_MAX 861
#define MIN_SPEED_LED ESC_MIN+100
