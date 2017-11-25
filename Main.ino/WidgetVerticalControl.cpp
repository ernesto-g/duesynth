#include "UTFT/UTFT.h"
#include "LCDManager.h"
#include "Images.h"

extern UTFT myGLCD;

static int state;

void widvc_startDrawingVerticalControl(WidgetVerticalControl* w)
{
    state=0;
}

int widvc_runDrawingVerticalControl(WidgetVerticalControl* w)
{
  int x, y, value;
  x = w->obj.x;
  y = w->obj.y;
  value = w->value;


  if(value<y || value > (y+171-20) )
    value = y;
  
    switch(state)
    {
      case 0: // clear background
      {
        
        myGLCD.setColor(0x42,0x42,0x42); 
        myGLCD.fillRect(x-14, y, x+33, y+85);
        state = 1; 
        break;
      }
      case 1: // clear background
      {
          myGLCD.setColor(0x42,0x42,0x42); 
          myGLCD.fillRect(x-14, y+85, x+33, y+171);
          state = 2;         
          break;
      }
      case 2: // draw line
      {
        /*
        myGLCD.drawBitmapStart(x,y, 11, 171,img_getImageControlBar1());
        state =3;
        */
        state =4;
        break;
      }
      case 3: // draw line
      {
          int r = myGLCD.drawBitmapRun();
          if(r==1)
            state = 4;        
        break;
      }
      case 4: // draw knob
      {
        myGLCD.drawBitmapStart(x-14,value, 33, 17,img_getImageControlBar0());
        state =5;
        break;
      }
      case 5:
      {
          int r = myGLCD.drawBitmapRun();
          if(r==1)
            state = 6;                
        break;
      }
      case 6:
        return 1;
    }         
  return 0;  
 
}


int widvc_touchEvent(WidgetVerticalControl* w, int x, int y)
{
     if( (w->obj.state&WIDGET_STATE_REDRAW)==0)
     {
        w->value = 240 - y;
        w->obj.state|=WIDGET_STATE_REDRAW;
     }
  return 0; // dont wait touch release to generate another event
}


