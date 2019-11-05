#ifndef bikeMenu_h
#define bikeMenu_h


class bikeMenu {
  public:
    bikeMenu(String menuStates[], int states, LiquidCrystal_I2C & lcd);
    bikeMenu();
    int changeMenuPosition(int change);
    void renderMenu();
    int getOffset();
  private:
    //Menu states
    String *_menuStates;
    int _numberOfStates;
    int _menuOffset;
    int _cursorOffset;

    //Menu state
    int _currentState;
    int calculateMenuOffset(int change);

    //References
    LiquidCrystal_I2C & lcdRef;
};

#endif
