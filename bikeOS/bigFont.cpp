/*
Custom large numbers that look hot
Credit to Michael Pilcher, adapted by Aaron Becker
 */

//big font library
#include <Arduino.h>
#include "bigFont.h"
#include "bigFontChars.h"
#include <LiquidCrystal_I2C.h>

//Constant data declaration

int bigFont::writeString(String str, int xPos, int yPos) {
  int strLen = str.length()+1; //optimize gang!

  char charBuffer[strLen];
  str.toCharArray(charBuffer, strLen);

  for (int i=0; i<strLen; i++) {
    Serial.println(charBuffer[i]);
    writeChar(charBuffer[i], xPos, yPos);
    xPos+=4;
  }

  return xPos; //returns new pos
}

void bigFont::init(LiquidCrystal_I2C & lcd) : lcdRef (lcd) {} //allow resetting of LCD reference

int bigFont::writeChar(char tW, int x, int y) {
  if (tW >= 65 && tW <= 90) {
    tW = tolower(tW);
  }
  switch (tW) { //lower case char, will only affect letters
    case '0':
    case 'o':
      custom0O(x, y);
      break;
    case '1':
      custom1(x, y);
      break;
    case '2':
      custom2(x, y);
      break;
    case '3':
      custom3(x, y);
      break;
    case '4':
      custom4(x, y);
      break;
    case '5':
      custom5(x, y);
      break;
    case '6':
      custom6(x, y);
      break;
    case '7':
      custom7(x, y);
      break;
    case '8':
      custom8(x, y);
      break;
    case '9':
      custom9(x, y);
      break;
    case 'a':
      customA(x, y);
      break;
    case 'b':
      customB(x, y);
      break;
    case 'c':
      customC(x, y);
      break;
    case 'd':
      customD(x, y);
      break;
    case 'e':
      customE(x, y);
      break;
    case 'f':
      customF(x, y);
      break;
    case 'g':
      customG(x, y);
      break;
    case 'h':
      customH(x, y);
      break;
    case 'i':
      customI(x, y);
      break;
    case 'j':
      customJ(x, y);
      break;
    case 'k':
      customK(x, y);
      break;
    case 'l':
      customL(x, y);
      break;
    case 'm':
      customM(x, y);
      break;
    case 'n':
      customN(x, y);
      break;
    case 'p':
      customP(x, y);
      break;
    case 'q':
      customQ(x, y);
      break;
    case 'r':
      customR(x, y);
      break;
    case 's':
      customS(x, y);
      break;
    case 't':
      customT(x, y);
      break;
    case 'u':
      customU(x, y);
      break;
    case 'v':
      customV(x, y);
      break;
    case 'w':
      customW(x, y);
      break;
    case 'x':
      customX(x, y);
      break;
    case 'y':
      customY(x, y);
      break;
    case 'z':
      customZ(x, y);
      break;
  }
  return x+4; //return new xPos
}

bigFont::bigFont(LiquidCrystal_I2C & lcd) : lcdRef (lcd) {
  //Assign segments write numbers
  lcdRef.createChar(8,C_LT);
  lcdRef.createChar(1,C_UB);
  lcdRef.createChar(2,C_RT);
  lcdRef.createChar(3,C_LL);
  lcdRef.createChar(4,C_LB);
  lcdRef.createChar(5,C_LR);
  lcdRef.createChar(6,C_UMB);
  lcdRef.createChar(7,C_LMB);
}
void bigFont::custom0O(int offsetX, int offsetY)
{ // uses segments to build the number 0
  lcdRef.setCursor(offsetX, offsetY); 
  lcdRef.write(8);  
  lcdRef.write(1); 
  lcdRef.write(2);
  lcdRef.setCursor(offsetX, offsetY+1); 
  lcdRef.write(3);  
  lcdRef.write(4);  
  lcdRef.write(5);
}

void bigFont::custom1(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(1);
  lcdRef.write(2);
  lcdRef.setCursor(offsetX+1, offsetY+1);
  lcdRef.write(255);
}

void bigFont::custom2(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(6);
  lcdRef.write(6);
  lcdRef.write(2);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(3);
  lcdRef.write(7);
  lcdRef.write(7);
}

void bigFont::custom3(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(6);
  lcdRef.write(6);
  lcdRef.write(2);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(7);
  lcdRef.write(7);
  lcdRef.write(5); 
}

void bigFont::custom4(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(3);
  lcdRef.write(4);
  lcdRef.write(2);
  lcdRef.setCursor(offsetX+2, offsetY+1);
  lcdRef.write(255);
}

void bigFont::custom5(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(255);
  lcdRef.write(6);
  lcdRef.write(6);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(7);
  lcdRef.write(7);
  lcdRef.write(5);
}

void bigFont::custom6(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(8);
  lcdRef.write(6);
  lcdRef.write(6);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(3);
  lcdRef.write(7);
  lcdRef.write(5);
}

void bigFont::custom7(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(1);
  lcdRef.write(1);
  lcdRef.write(2);
  lcdRef.setCursor(offsetX+1, offsetY+1);
  lcdRef.write(8);
}

void bigFont::custom8(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(8);
  lcdRef.write(6);
  lcdRef.write(2);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(3);
  lcdRef.write(7);
  lcdRef.write(5);
}

void bigFont::custom9(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(8);
  lcdRef.write(6);
  lcdRef.write(2);
  lcdRef.setCursor(offsetX+2, offsetY+1);
  lcdRef.write(255);
}

void bigFont::customA(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(8);
  lcdRef.write(6);
  lcdRef.write(2);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(255);
  lcdRef.write(254);
  lcdRef.write(255);
}

void bigFont::customB(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(255);
  lcdRef.write(6);
  lcdRef.write(5);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(255);
  lcdRef.write(7);
  lcdRef.write(2);
}

void bigFont::customC(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(8);
  lcdRef.write(1);
  lcdRef.write(1);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(3);
  lcdRef.write(4);
  lcdRef.write(4);
}

void bigFont::customD(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY); 
  lcdRef.write(255);  
  lcdRef.write(1); 
  lcdRef.write(2);
  lcdRef.setCursor(offsetX, offsetY+1); 
  lcdRef.write(255);  
  lcdRef.write(4);  
  lcdRef.write(5);
}

void bigFont::customE(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(255);
  lcdRef.write(6);
  lcdRef.write(6);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(255);
  lcdRef.write(7);
  lcdRef.write(7); 
}

void bigFont::customF(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(255);
  lcdRef.write(6);
  lcdRef.write(6);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(255);
}

void bigFont::customG(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(8);
  lcdRef.write(1);
  lcdRef.write(1);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(3);
  lcdRef.write(4);
  lcdRef.write(2);
}

void bigFont::customH(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(255);
  lcdRef.write(4);
  lcdRef.write(255);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(255);
  lcdRef.write(254);
  lcdRef.write(255); 
}

void bigFont::customI(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(1);
  lcdRef.write(255);
  lcdRef.write(1);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(4);
  lcdRef.write(255);
  lcdRef.write(4);
}

void bigFont::customJ(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX+2, offsetY);
  lcdRef.write(255);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(4);
  lcdRef.write(4);
  lcdRef.write(5);
}

void bigFont::customK(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(255);
  lcdRef.write(4);
  lcdRef.write(5);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(255);
  lcdRef.write(254);
  lcdRef.write(2); 
}

void bigFont::customL(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(255);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(255);
  lcdRef.write(4);
  lcdRef.write(4);
}

void bigFont::customM(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(8);
  lcdRef.write(3);
  lcdRef.write(5);
  lcdRef.write(2);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(255);
  lcdRef.write(254);
  lcdRef.write(254);
  lcdRef.write(255);
}

void bigFont::customN(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(8);
  lcdRef.write(2);
  lcdRef.write(254);
  lcdRef.write(255);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(255);
  lcdRef.write(254);
  lcdRef.write(3);
  lcdRef.write(5);
}

void bigFont::customP(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(255);
  lcdRef.write(6);
  lcdRef.write(2);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(255);
}

void bigFont::customQ(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(8);
  lcdRef.write(1);
  lcdRef.write(2);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(3);
  lcdRef.write(4);
  lcdRef.write(255);
  lcdRef.write(4);
}

void bigFont::customR(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(255);
  lcdRef.write(6);
  lcdRef.write(2);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(255);
  lcdRef.write(254);
  lcdRef.write(2); 
}

void bigFont::customS(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(8);
  lcdRef.write(6);
  lcdRef.write(6);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(7);
  lcdRef.write(7);
  lcdRef.write(5);
}

void bigFont::customT(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(1);
  lcdRef.write(255);
  lcdRef.write(1);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(254);
  lcdRef.write(255);
}

void bigFont::customU(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY); 
  lcdRef.write(255);  
  lcdRef.write(254); 
  lcdRef.write(255);
  lcdRef.setCursor(offsetX, offsetY+1); 
  lcdRef.write(3);  
  lcdRef.write(4);  
  lcdRef.write(5);
}

void bigFont::customV(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY); 
  lcdRef.write(3);  
  lcdRef.write(254);
  lcdRef.write(254); 
  lcdRef.write(5);
  lcdRef.setCursor(offsetX+1, offsetY+1); 
  lcdRef.write(2);  
  lcdRef.write(8);
}

void bigFont::customW(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(255);
  lcdRef.write(254);
  lcdRef.write(254);
  lcdRef.write(255);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(3);
  lcdRef.write(8);
  lcdRef.write(2);
  lcdRef.write(5);
}

void bigFont::customX(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(3);
  lcdRef.write(4);
  lcdRef.write(5);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(8);
  lcdRef.write(254);
  lcdRef.write(2); 
}

void bigFont::customY(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(3);
  lcdRef.write(4);
  lcdRef.write(5);
  lcdRef.setCursor(offsetX+1, offsetY+1);
  lcdRef.write(255);
}

void bigFont::customZ(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(1);
  lcdRef.write(6);
  lcdRef.write(5);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(8);
  lcdRef.write(7);
  lcdRef.write(4);
}

void bigFont::customQM(int offsetX, int offsetY) //Question mark
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(1);
  lcdRef.write(6);
  lcdRef.write(2);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(254);
  lcdRef.write(7);
}

void bigFont::customSM(int offsetX, int offsetY)
{
  lcdRef.setCursor(offsetX, offsetY);
  lcdRef.write(255);
  lcdRef.setCursor(offsetX, offsetY+1);
  lcdRef.write(7);
}

