#ifndef bikeMenu_h
#define bikeMenu_h


class bikeMenu {
  public:
    bikeMenu(String[] menuStates, int states);
    void init(LiquidCrystal_I2C & lcd, bigFont & customFont);
    int changeMenuPosition(int change);
    void renderMenu(boolean forceRedraw);
  private:
    //Menu states
    String[] _menuStates;
    int _states;
    int _menuOffset;
    int _cursorOffset;

    //Menu state
    int _currentState;
    int calculateMenuOffset(int change);

    //References
    LiquidCrystal_I2C & lcdRef;
    bigFont & fontRef;
};

#endif
