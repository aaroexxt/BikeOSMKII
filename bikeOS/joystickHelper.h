#ifndef joystickHelper_h
#define joystickHelper_h

struct joystickPosition {
  int x;
  int y;
};

class joystickHelper {
  public:
    joystickHelper(int xPin, int yPin, int swPin);
    int x;
    int y;
    boolean pressed;
    boolean isPressed();
    joystickPosition getPosition();
    void update();
  private:
    int xPin;
    int yPin;
    int swPin;
    long lastPressTime;
    int lastPressState = LOW;
};

#endif
