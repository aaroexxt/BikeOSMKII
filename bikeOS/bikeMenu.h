#ifndef bikeMenu_h
#define bikeMenu_h


class bikeMenu {
  public:
    bikeMenu(String[] menuStates, int states);
    void init(LiquidCrystal_I2C & lcd, bigFont & customFont, DHT & dht, joystickHelper & joystickHelper);
    void transitionState(int newState);
    void transitonState(int newState, boolean forceRedraw);
    void renderMenu(boolean forceRedraw);
  private:
    //Menu states
    String[] _menuStates;
    int _states;

    //Menu state
    int currentState;

    //References
    LiquidCrystal_I2C & lcdRef;
    bigFont & fontRef;
    DHT & dhtRef;
    joystickHelper & joystickRef;
};

#endif
