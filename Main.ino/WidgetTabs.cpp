#include "UTFT/UTFT.h"
#include "LCDManager.h"
#include "Images.h"
#include "WidgetTabs.h"

extern UTFT myGLCD;

static int state;
 
#define TAB_MAX 3 // (0..3)

void widtab_initWidget(WidgetTabs* w,int tabNumber, void (*callbackTouchEvent)(int tabNumber, int prevTabNumber) )
{
  w->obj.x = 0;
  w->obj.y = 0;
  w->obj.w = 320;
  w->obj.h = 36;
  w->obj.touchAreaX0=0;
  w->obj.touchAreaX1=320;
  w->obj.touchAreaY0=0;
  w->obj.touchAreaY1=36;
  w->obj.type = WIDGET_TYPE_TABS;
  w->obj.state = WIDGET_STATE_REDRAW;
  w->currentTabNumber=tabNumber;
  w->callbackTouchEvent = callbackTouchEvent;
} 


void widtab_setTabNumber(WidgetTabs* w, int tabNumber)
{
    if(tabNumber<=TAB_MAX && tabNumber>=0)
    {
        w->currentTabNumber=tabNumber;        
    }
}

void widtab_startDrawingTabs(WidgetTabs* w)
{
    state=0;
}

int widtab_runDrawingTabs(WidgetTabs* w)
{
    switch(state)
    {
        case 0:
        {
          myGLCD.drawBitmapStart(w->obj.x,w->obj.y,w->obj.w,w->obj.h,img_getImageTabs(w->currentTabNumber));
          state=1;
          break;
        }
        case 1:
        {
          if(myGLCD.drawBitmapRun()==1)
          {
              // drawing finished
              return 1;
          }          
          break;
        }
    }
    return 0;
}



int widtab_touchEvent(WidgetTabs* w,int x,int y)
{
    int prevTab = w->currentTabNumber;

    if(x>=0 && x<= 70)
      w->currentTabNumber=0;
    
    if(x>=80 && x<= 160)
      w->currentTabNumber=1;

    if(x>=170 && x<= 235)
      w->currentTabNumber=2;

    if(x>=240 && x<= 320)
      w->currentTabNumber=3;

    if(w->currentTabNumber!=prevTab)
    {     
      if(w->callbackTouchEvent!=NULL)
      {
        w->callbackTouchEvent(w->currentTabNumber,prevTab);
        w->obj.state|= WIDGET_STATE_REDRAW;
      }
    }
    
    return 1; // wait release
}


