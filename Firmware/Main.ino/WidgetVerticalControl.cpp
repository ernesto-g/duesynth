/*  MINIBRUTE Clone with arduino DUE
    Copyright (C) 2018  Ernesto Gigliotti <ernestogigliotti@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "UTFT/UTFT.h"
#include "LCDManager.h"
#include "Images.h"
#include "WidgetVerticalControl.h"

#define CONTROL_H   171

extern UTFT myGLCD;

static int state;

void widvc_initWidget(WidgetVerticalControl* w,int x, int y,int value)
{
  w->obj.x = x;
  w->obj.y = y;
  w->obj.w = 50;
  w->obj.h = 171;
  w->obj.touchAreaX0=x - 30;
  w->obj.touchAreaX1=x+50;
  w->obj.touchAreaY0=y-25;
  w->obj.touchAreaY1=y+171;
  w->obj.type = WIDGET_TYPE_VERTICAL_CONTROL;
  w->obj.state = WIDGET_STATE_REDRAW;
  widvc_setMidiValue(w,value);
} 

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

void widvc_clearWidgetArea(WidgetVerticalControl* w)
{
  // delete all
   myGLCD.fillRect(w->obj.x-14, w->obj.y, w->obj.x+33, w->obj.y+CONTROL_H);
}

