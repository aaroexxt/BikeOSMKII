#ifndef bigFont_h
#define bigFont_h

class bigFont {
  public:
    bigFont(LiquidCrystal_I2C & lcd);
    int writeString(String characters);
  private:
    //Lcd reference; to allow direct writing to lcd
    LiquidCrystal_I2C lcdRef;

    //Custom character storage
    /*byte C_LT;
    byte C_UB;
    byte C_RT;
    byte C_LL;
    byte C_LB;
    byte C_LR;
    byte C_UMB;
    byte C_LMB;*/

    //All letter generating functions
    void customA(int offsetX, int offsetY);
    void customB(int offsetX, int offsetY);
    void customC(int offsetX, int offsetY);
    void customD(int offsetX, int offsetY);
    void customE(int offsetX, int offsetY);
    void customF(int offsetX, int offsetY);
    void customG(int offsetX, int offsetY);
    void customH(int offsetX, int offsetY);
    void customI(int offsetX, int offsetY);
    void customJ(int offsetX, int offsetY);
    void customK(int offsetX, int offsetY);
    void customL(int offsetX, int offsetY);
    void customM(int offsetX, int offsetY);
    void customN(int offsetX, int offsetY);
    void custom0O(int offsetX, int offsetY);
    void customP(int offsetX, int offsetY);
    void customQ(int offsetX, int offsetY);
    void customR(int offsetX, int offsetY);
    void customS(int offsetX, int offsetY);
    void customT(int offsetX, int offsetY);
    void customU(int offsetX, int offsetY);
    void customV(int offsetX, int offsetY);
    void customW(int offsetX, int offsetY);
    void customX(int offsetX, int offsetY);
    void customY(int offsetX, int offsetY);
    void customZ(int offsetX, int offsetY);
    void customQM(int offsetX, int offsetY);
    void customSM(int offsetX, int offsetY);


};

#endif
