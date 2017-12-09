// LCD and Touch libraries
#include "UTFT/UTFT.h"
#include "Fonts.h"
#include "Images.h"
#include "URTouch/URTouch.h"
#include "LCDManager.h"

#include "WindowsManager.h"
#include "WidgetVerticalControl.h"
#include "WidgetTabs.h"

extern UTFT myGLCD;

// Private functions
static void eventTabPressed(int tabNumber, int prevTabNumber);
static void showADSR2(void);
static void showADSR1(void);
//__________________

// Widgets definitions

static WidgetTabs  widgetTabs;

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
    // show logo
    myGLCD.fillScr(0x42, 0x42, 0x42);   
    // TODO
     

    // while loading all widgets
    widtab_initWidget(&widgetTabs,0,eventTabPressed); // TAB 0  
    
    widvc_initWidget(&controlAttack1,40,50,64);
    widvc_initWidget(&controlDecay1,110,50,64);
    widvc_initWidget(&controlSustain1,180,50,64);
    widvc_initWidget(&controlRelease1,250,50,64);

    widvc_initWidget(&controlAttack2,40,50,64);
    win_hideWidget((Widget*)&controlAttack2);
    widvc_initWidget(&controlDecay2,110,50,64);
    win_hideWidget((Widget*)&controlDecay2);
    widvc_initWidget(&controlSustain2,180,50,64);
    win_hideWidget((Widget*)&controlSustain2);
    widvc_initWidget(&controlRelease2,250,50,64);  
    win_hideWidget((Widget*)&controlRelease2);  
        
    win_addWidget(0, (Widget*)&widgetTabs);
    
    win_addWidget(1, (Widget*)&controlAttack1);
    win_addWidget(2, (Widget*)&controlDecay1);
    win_addWidget(3, (Widget*)&controlSustain1);
    win_addWidget(4, (Widget*)&controlRelease1);

    win_addWidget(5, (Widget*)&controlAttack2);
    win_addWidget(6, (Widget*)&controlDecay2);
    win_addWidget(7, (Widget*)&controlSustain2);
    win_addWidget(8, (Widget*)&controlRelease2);
    
    //_____________________________________________


    
}


static void showADSR1(void)
{
    win_showWidget((Widget*)&controlAttack1);
    win_showWidget((Widget*)&controlDecay1);
    win_showWidget((Widget*)&controlSustain1);
    win_showWidget((Widget*)&controlRelease1);
      
}
static void showADSR2(void)
{
    win_showWidget((Widget*)&controlAttack2);
    win_showWidget((Widget*)&controlDecay2);
    win_showWidget((Widget*)&controlSustain2);
    win_showWidget((Widget*)&controlRelease2);
      
}

static void eventTabPressed(int tabNumber, int prevTabNumber)
{
  myGLCD.setColor(0x42,0x42,0x42); 
    
  switch(prevTabNumber)
  {
    case 0:
    {
      win_hideWidget((Widget*)&controlAttack1);
      widvc_clearWidgetArea(&controlAttack1);
      win_hideWidget((Widget*)&controlDecay1);
      widvc_clearWidgetArea(&controlDecay1);
      win_hideWidget((Widget*)&controlSustain1);
      widvc_clearWidgetArea(&controlSustain1);
      win_hideWidget((Widget*)&controlRelease1);
      widvc_clearWidgetArea(&controlRelease1);
      break;
    }
    case 1:
    {
      win_hideWidget((Widget*)&controlAttack2);
      widvc_clearWidgetArea(&controlAttack2);
      win_hideWidget((Widget*)&controlDecay2);
      widvc_clearWidgetArea(&controlDecay2);
      win_hideWidget((Widget*)&controlSustain2);
      widvc_clearWidgetArea(&controlSustain2);
      win_hideWidget((Widget*)&controlRelease2);
      widvc_clearWidgetArea( &controlRelease2);
      break;
    }    
  }
  

  switch(tabNumber)
  {
    case 0:
    {
      showADSR1();
      break;
    }
    case 1:
    {
      showADSR2();
      break;
    }    
  }
}


int wm_getControlMidiValueById(int controlId)
{
  switch(controlId)
  {
      case WIN_MAN_CONTROL_ATTACK1: return widvc_getMidiValue(&controlAttack1);
      case WIN_MAN_CONTROL_DECAY1: return widvc_getMidiValue(&controlDecay1);
      case WIN_MAN_CONTROL_SUSTAIN1: return widvc_getMidiValue(&controlSustain1);
      case WIN_MAN_CONTROL_RELEASE1: return widvc_getMidiValue(&controlRelease1);
      
      case WIN_MAN_CONTROL_ATTACK2: return widvc_getMidiValue(&controlAttack2);
      case WIN_MAN_CONTROL_DECAY2: return widvc_getMidiValue(&controlDecay2);
      case WIN_MAN_CONTROL_SUSTAIN2: return widvc_getMidiValue(&controlSustain2);
      case WIN_MAN_CONTROL_RELEASE2: return widvc_getMidiValue(&controlRelease2);
  }
  return -1;
}


