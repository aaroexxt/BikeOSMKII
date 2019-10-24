/*
Custom large numbers that look hot
Credit to Michael Pilcher, adapted by Aaron Becker
 */

//big font library
#include <Arduino.h>
#include "bigFont.h"

bigFont::bigFont(LiquidCrystal_I2C & lcd) : lcdRef (lcd) {
  //Assign segments write numbers
  lcdRef.createChar(8,LT);
  lcdRef.createChar(1,UB);
  lcdRef.createChar(2,RT);
  lcdRef.createChar(3,LL);
  lcdRef.createChar(4,LB);
  lcdRef.createChar(5,LR);
  lcdRef.createChar(6,UMB);
  lcdRef.createChar(7,LMB);
}

int bigFont::writeChar(int char, int x, int y) {
  
}

// the 8 arrays that form each segment of the custom numbers
byte C_LT[8] = {
  B00111,
  B01111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte C_UB[8] = {
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte C_RT[8] = {
  B11100,
  B11110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte C_LL[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B01111,
  B00111
};
byte C_LB[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111
};
byte C_LR[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11110,
  B11100
};
byte C_UMB[8] = {
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111
};
byte C_LMB[8] = {
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111
};

void bigFont::custom0O(int offsetX, int offsetY)
{ // uses segments to build the number 0
  lcd.setCursor(offsetX, offsetY); 
  lcd.write(8);  
  lcd.write(1); 
  lcd.write(2);
  lcd.setCursor(offsetX, offsetY+1); 
  lcd.write(3);  
  lcd.write(4);  
  lcd.write(5);
}

void bigFont::custom1(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(offsetX+1,1);
  lcd.write(255);
}

void bigFont::custom2(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(6);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(3);
  lcd.write(7);
  lcd.write(7);
}

void bigFont::custom3(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(6);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(7);
  lcd.write(7);
  lcd.write(5); 
}

void bigFont::custom4(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(3);
  lcd.write(4);
  lcd.write(2);
  lcd.setCursor(offsetX+2, 1);
  lcd.write(255);
}

void bigFont::custom5(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(255);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(7);
  lcd.write(7);
  lcd.write(5);
}

void bigFont::custom6(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(8);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(3);
  lcd.write(7);
  lcd.write(5);
}

void bigFont::custom7(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(1);
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(offsetX+1, 1);
  lcd.write(8);
}

void bigFont::custom8(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(8);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(3);
  lcd.write(7);
  lcd.write(5);
}

void bigFont::custom9(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(8);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(offsetX+2, 1);
  lcd.write(255);
}

void bigFont::customA(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(8);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(255);
  lcd.write(254);
  lcd.write(255);
}

void bigFont::customB(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(255);
  lcd.write(6);
  lcd.write(5);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(255);
  lcd.write(7);
  lcd.write(2);
}

void bigFont::customC(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(8);
  lcd.write(1);
  lcd.write(1);
  lcd.setCursor(offsetX,1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(4);
}

void bigFont::customD(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX, offsetY); 
  lcd.write(255);  
  lcd.write(1); 
  lcd.write(2);
  lcd.setCursor(offsetX, offsetY+1); 
  lcd.write(255);  
  lcd.write(4);  
  lcd.write(5);
}

void bigFont::customE(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(255);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(255);
  lcd.write(7);
  lcd.write(7); 
}

void bigFont::customF(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(255);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(255);
}

void bigFont::customG(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(8);
  lcd.write(1);
  lcd.write(1);
  lcd.setCursor(offsetX,1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(2);
}

void bigFont::customH(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(255);
  lcd.write(4);
  lcd.write(255);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(255);
  lcd.write(254);
  lcd.write(255); 
}

void bigFont::customI(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(1);
  lcd.write(255);
  lcd.write(1);
  lcd.setCursor(offsetX,1);
  lcd.write(4);
  lcd.write(255);
  lcd.write(4);
}

void bigFont::customJ(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX+2,0);
  lcd.write(255);
  lcd.setCursor(offsetX,1);
  lcd.write(4);
  lcd.write(4);
  lcd.write(5);
}

void bigFont::customK(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(255);
  lcd.write(4);
  lcd.write(5);
  lcd.setCursor(offsetX,1);
  lcd.write(255);
  lcd.write(254);
  lcd.write(2); 
}

void bigFont::customL(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(255);
  lcd.setCursor(offsetX,1);
  lcd.write(255);
  lcd.write(4);
  lcd.write(4);
}

void bigFont::customM(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(8);
  lcd.write(3);
  lcd.write(5);
  lcd.write(2);
  lcd.setCursor(offsetX,1);
  lcd.write(255);
  lcd.write(254);
  lcd.write(254);
  lcd.write(255);
}

void bigFont::customN(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(8);
  lcd.write(2);
  lcd.write(254);
  lcd.write(255);
  lcd.setCursor(offsetX,1);
  lcd.write(255);
  lcd.write(254);
  lcd.write(3);
  lcd.write(5);
}

void bigFont::customP(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(255);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(255);
}

void bigFont::customQ(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(8);
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(255);
  lcd.write(4);
}

void bigFont::customR(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(255);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(offsetX,1);
  lcd.write(255);
  lcd.write(254);
  lcd.write(2); 
}

void bigFont::customS(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(8);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(7);
  lcd.write(7);
  lcd.write(5);
}

void bigFont::customT(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(1);
  lcd.write(255);
  lcd.write(1);
  lcd.setCursor(offsetX,1);
  lcd.write(254);
  lcd.write(255);
}

void bigFont::customU(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX, offsetY); 
  lcd.write(255);  
  lcd.write(254); 
  lcd.write(255);
  lcd.setCursor(offsetX, offsetY+1); 
  lcd.write(3);  
  lcd.write(4);  
  lcd.write(5);
}

void bigFont::customV(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX, offsetY); 
  lcd.write(3);  
  lcd.write(254);
  lcd.write(254); 
  lcd.write(5);
  lcd.setCursor(offsetX+1, 1); 
  lcd.write(2);  
  lcd.write(8);
}

void bigFont::customW(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(255);
  lcd.write(254);
  lcd.write(254);
  lcd.write(255);
  lcd.setCursor(offsetX,1);
  lcd.write(3);
  lcd.write(8);
  lcd.write(2);
  lcd.write(5);
}

void bigFont::customX(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
  lcd.setCursor(offsetX,1);
  lcd.write(8);
  lcd.write(254);
  lcd.write(2); 
}

void bigFont::customY(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
  lcd.setCursor(offsetX+1,1);
  lcd.write(255);
}

void bigFont::customZ(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(1);
  lcd.write(6);
  lcd.write(5);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(8);
  lcd.write(7);
  lcd.write(4);
}

void bigFont::customQM(int offsetX, int offsetY) //Question mark
{
  lcd.setCursor(offsetX,0);
  lcd.write(1);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(254);
  lcd.write(7);
}

void bigFont::customSM(int offsetX, int offsetY)
{
  lcd.setCursor(offsetX,0);
  lcd.write(255);
  lcd.setCursor(offsetX, offsetY+1);
  lcd.write(7);
}

