// LCD and Touch libraries
#include "UTFT/UTFT.h"
#include "Fonts.h"
#include "Images.h"
#include "URTouch/URTouch.h"
#include "LCDManager.h"

#include "WidgetVerticalControl.h"

extern UTFT myGLCD;

// Widgets definitions

// ADSR1 Window
static WidgetVerticalControl controlAttack1;
static WidgetVerticalControl controlDecay1;
static WidgetVerticalControl controlSustain1;
static WidgetVerticalControl controlRelease1;

// ADSR2 Window
static WidgetVerticalControl controlAttack2;
static WidgetVerticalControl controlDecay2;
static WidgetVerticalControl controlSustain2;
static WidgetVerticalControl controlRelease2;
//_____________________



void wm_showBoot(void)
{
  
}

void wm_showMainWindow(void)
{ 
  widvc_initWidget(&controlAttack1,40,50,64);
  widvc_initWidget(&controlDecay1,110,50,64);
  widvc_initWidget(&controlSustain1,180,50,64);
  widvc_initWidget(&controlRelease1,250,50,64);
  
  
  win_addWidget(0, (Widget*)&controlAttack1);
  win_addWidget(1, (Widget*)&controlDecay1);
  win_addWidget(2, (Widget*)&controlSustain1);
  win_addWidget(3, (Widget*)&controlRelease1);
}

void wm_showADSR1(void)
{

}

