#include <LiquidCrystal_I2C.h>
#ifndef menuHelper_h
#define menuHelper_h

class menuHelper {
  public:
    menuHelper(LiquidCrystal_I2C *LCDreference, String menuStates[], int numberOfStates);
    void renderMenu();
    void changeMenuPosition(int change);
    int getMenuOffset();
    int displayHeight = 4;
  private:
    int calculateMenuOffset(int change);
    int menuOffset = 0; //menu offset from top
    int cursorOffset = 0; //cursor offset from top
    int numberOfStates = 0;
    String *menuStates[];
    LiquidCrystal_I2C *LCDreference;
};

#endif
