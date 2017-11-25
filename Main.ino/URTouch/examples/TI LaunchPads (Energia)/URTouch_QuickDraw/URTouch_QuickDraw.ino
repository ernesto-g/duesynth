// URTouch_QuickDraw 
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program is a quick demo of how to use the library.
//
// This program requires the UTFT library.
//
// It is assumed that the display module is connected to an
// appropriate shield or that you know how to change the pin 
// numbers in the setup.
//

#include <UTFT.h>
#include <URTouch.h>

// Initialize display
// ------------------
// Remember to change the model parameter to suit your display module!
UTFT    myGLCD(ITDB32S,15,18,11,32);

// Initialize touchscreen
// ----------------------
URTouch  myTouch(31, 13, 19, 28, 17);

void setup()
{
  myGLCD.InitLCD();
  myGLCD.clrScr();

  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
}

void loop()
{
  long x, y;
  
  while (myTouch.dataAvailable() == true)
  {
    myTouch.read();
    x = myTouch.getX();
    y = myTouch.getY();
    if ((x!=-1) and (y!=-1))
    {
      myGLCD.drawPixel (x, y);
    }
  }
}

