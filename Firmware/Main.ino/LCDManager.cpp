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
#include "UTFT/UTFT.cpp"
#include "URTouch/URTouch.h"
#include "URTouch/URTouch.cpp"

#include "LCDManager.h"
#include "URTouch/URTouch.h"
#include "WidgetVerticalControl.h"
#include "WidgetTabs.h"

#define WIDGETS_LEN 16

#define STATE_DRAW_IDLE           0
#define STATE_DRAW_CHECK_WIDGET   1
#define STATE_DRAW_START          2
#define STATE_DRAW_WAIT           3
#define STATE_DRAW_FINISHED       4

#define TOUCH_STATE_IDLE    0
#define TOUCH_STATE_READ    1
#define TOUCH_STATE_READ_X  2
#define TOUCH_STATE_READ_Y  3
#define TOUCH_STATE_ANALIZE 4
#define TOUCH_STATE_WAIT_RELEASE  5
#define TOUCH_STATE_WIDGET_EVENT  6
#define TOUCH_STATE_WAIT_FOR_NEXT_TOUCH_CHECK 7

#define TOUCH_SCAN_PERIOD   50 // period for touch scanning. in ms


static void startDrawingWidget(Widget* w);
static int runDrawingWidget(Widget* w);
static Widget* getPressedWidget(int x, int y);
static int generateTouchEventForWidget(Widget* w,int x, int y);


extern URTouch  myTouch;
static Widget* widgets[WIDGETS_LEN];
static int stateDraw;
static int indexDraw;
static volatile int counterWaitForNextTouch;

void win_init(void)
{
  int i;
  for(i=0; i<WIDGETS_LEN; i++)
  {
    widgets[i]=NULL;
  }
  stateDraw = STATE_DRAW_IDLE;
}

void win_sysTick(void)
{
    counterWaitForNextTouch--;
}

void win_addWidget(int i, Widget* w)
{
    widgets[i] = w;
}

void win_hideWidget(Widget* w)
{
  w->state|=WIDGET_STATE_HIDDEN;
}
void win_showWidget(Widget* w)
{
  w->state&=(~WIDGET_STATE_HIDDEN);
  w->state|=WIDGET_STATE_REDRAW;
}



void win_drawAll(void)
{
    switch(stateDraw)
    {
      case STATE_DRAW_IDLE:
      {
        indexDraw = 0;
        stateDraw = STATE_DRAW_CHECK_WIDGET;
        break;
      }
      case STATE_DRAW_CHECK_WIDGET:
      {
        if(widgets[indexDraw]!=NULL && ((widgets[indexDraw]->state&WIDGET_STATE_REDRAW)==WIDGET_STATE_REDRAW) && ((widgets[indexDraw]->state&WIDGET_STATE_HIDDEN)!=WIDGET_STATE_HIDDEN) ) 
        {
            stateDraw = STATE_DRAW_START;     
        }
        else
        {
            stateDraw = STATE_DRAW_FINISHED;
        }
        break;
      }
      case STATE_DRAW_START:
      {
          startDrawingWidget(widgets[indexDraw]);
          stateDraw = STATE_DRAW_WAIT;         
      }
      case STATE_DRAW_WAIT:
      {
        if(runDrawingWidget(widgets[indexDraw])==1)
        {
          // finished
          widgets[indexDraw]->state&=(~WIDGET_STATE_REDRAW);
          stateDraw = STATE_DRAW_FINISHED; 
        }
        break;
      }
      case STATE_DRAW_FINISHED:
      {
          indexDraw++;
          if(indexDraw>=WIDGETS_LEN)
          {
            indexDraw=0;            
          }
          stateDraw = STATE_DRAW_CHECK_WIDGET;
          break;      
      }
    }
}


static void startDrawingWidget(Widget* w)
{
  switch(w->type)
  {
    case WIDGET_TYPE_VERTICAL_CONTROL:
    {
      widvc_startDrawingVerticalControl((WidgetVerticalControl*)w);
      break;
    }
    case WIDGET_TYPE_TABS:
    {
      widtab_startDrawingTabs((WidgetTabs*)w);
      break;
    }
  }
}

static int runDrawingWidget(Widget* w)
{
  switch(w->type)
  {
    case WIDGET_TYPE_VERTICAL_CONTROL:
    {
      return widvc_runDrawingVerticalControl((WidgetVerticalControl*)w);
      break;
    }
    case WIDGET_TYPE_TABS:
    {
      return widtab_runDrawingTabs((WidgetTabs*)w);
      break;
    }
  }  
    return 1;
}



void win_touchManager(void)
{
  static int touchState=TOUCH_STATE_IDLE;
  static int x,y;
  static Widget* w;
  int i;
    
  switch(touchState)
  {
    case TOUCH_STATE_IDLE:
    {
      if(myTouch.dataAvailable())
      {
        touchState = TOUCH_STATE_READ;
      }
      break;
    }
    case TOUCH_STATE_READ:
    {
        //x: entre 5 y 319
        //y: entre 224 y 13
        myTouch.read();
        touchState = TOUCH_STATE_READ_X;
        break;
    }
    case TOUCH_STATE_READ_X:
    {
      x=myTouch.getX();
      if(x==-1){
        touchState = TOUCH_STATE_IDLE;
        break; 
      }
      x = x-5;
      if(x<0)
        x=0;
      
      x = (x*320)/311;              
      touchState = TOUCH_STATE_READ_Y;      
      break;
    }
    case TOUCH_STATE_READ_Y:
    {
      y=myTouch.getY();
      if(y==-1)
      {
        touchState = TOUCH_STATE_IDLE;
        break; 
      }
      y = 224 - y;
      if(y<0)
        y=0;

      y = (y*240)/210;
        
      touchState = TOUCH_STATE_ANALIZE;
      break;
    }
    case TOUCH_STATE_ANALIZE:
    {
        w = getPressedWidget(x,y);
        if(w==NULL)
        {
            touchState = TOUCH_STATE_WAIT_FOR_NEXT_TOUCH_CHECK;
            counterWaitForNextTouch=TOUCH_SCAN_PERIOD;          }
        else
          touchState = TOUCH_STATE_WIDGET_EVENT;
        break;
    }
    case TOUCH_STATE_WAIT_RELEASE:
    {
      if(myTouch.dataAvailable()==0)
      {
        touchState = TOUCH_STATE_WAIT_FOR_NEXT_TOUCH_CHECK;
        counterWaitForNextTouch=TOUCH_SCAN_PERIOD;       
      } 
      break;
    }
    case TOUCH_STATE_WIDGET_EVENT:
    {
        if(generateTouchEventForWidget(w,x,y))
          touchState = TOUCH_STATE_WAIT_RELEASE;
        else
        {
          touchState = TOUCH_STATE_WAIT_FOR_NEXT_TOUCH_CHECK;
          counterWaitForNextTouch=TOUCH_SCAN_PERIOD;                 
        }
        break;
    }
    case TOUCH_STATE_WAIT_FOR_NEXT_TOUCH_CHECK:
    {
      if(counterWaitForNextTouch<=0)
        touchState = TOUCH_STATE_IDLE; 
      break;
    }
  }
}

static int generateTouchEventForWidget(Widget* w,int x, int y)
{
  switch(w->type)
  {
    case WIDGET_TYPE_VERTICAL_CONTROL:
    {
      return widvc_touchEvent((WidgetVerticalControl*)w,x,y);
      break;
    }
    case WIDGET_TYPE_TABS:
    {
      return widtab_touchEvent((WidgetTabs*)w,x,y);
      break;
    }    
  }  
    return 1; // wait release 
}

static Widget* getPressedWidget(int x, int y)
{
  int i;
  Widget* w=NULL;
  for(i=0; i<WIDGETS_LEN; i++)
  {
    w = widgets[i];
    if(w!=NULL && ((w->state&WIDGET_STATE_HIDDEN)!=WIDGET_STATE_HIDDEN) )
    {
        if(x >= w->touchAreaX0 && x<=w->touchAreaX1)
        {
            if(y >= w->touchAreaY0 && y<=w->touchAreaY1)
            {
              return w;
            }
        }
    }
  }
  return NULL;
}


