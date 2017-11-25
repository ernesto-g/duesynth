#include "UTFT/UTFT.h"
#include "LCDManager.h"
#include "Images.h"

#define CONTROL_H   171

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

  if(value<y)  
    value = y;
  if( value > (y+CONTROL_H) )
    value = (y+CONTROL_H) ;
  if(value<0)
    value=0;
    
    switch(state)
    {
      case 0: // clear background
      {
        myGLCD.setColor(0x42,0x42,0x42); 
        if(w->prevValue==-1)
        { // delete all
          myGLCD.fillRect(x-14, y, x+33, y+85);
          state = 1;
        }
        else
        {
            // delete previous knob
            myGLCD.fillRect(x-14,w->prevValue, x+33-14, w->prevValue+17);
            state = 2;
        }
        break;
      }
      case 1: // clear background
      {
          myGLCD.setColor(0x42,0x42,0x42); 
          myGLCD.fillRect(x-14, y+85, x+33, y+CONTROL_H);
          state = 2;         
          break;
      }
      case 2: // draw line
      {
        myGLCD.drawBitmapStart(x,y, 11, CONTROL_H,img_getImageControlBar1());
        state =3; 
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
        w->prevValue = value;
        
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

int widvc_getMidiValue(WidgetVerticalControl* w)
{
    return 127 - (((w->prevValue - w->obj.y)*127)/CONTROL_H);    
}

void widvc_setMidiValue(WidgetVerticalControl* w, int midiValue)
{
    w->prevValue = (w->obj.y + (( (127-midiValue) *CONTROL_H)/127));
    w->value = w->prevValue;
}

int widvc_touchEvent(WidgetVerticalControl* w, int x, int y)
{
    if( (w->obj.state&WIDGET_STATE_REDRAW)!=WIDGET_STATE_REDRAW )
    {
      w->value = y;
      w->obj.state|=WIDGET_STATE_REDRAW;
    }
    return 0; // dont wait touch release to generate another event
}


