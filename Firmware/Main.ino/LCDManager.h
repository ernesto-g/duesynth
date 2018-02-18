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

struct S_Widget
{
  int type;
  int x;
  int y;
  int w;
  int h;  
  int state;
  int touchAreaX0;
  int touchAreaY0;
  int touchAreaX1;
  int touchAreaY1;  
};
typedef struct S_Widget Widget;

struct S_WidgetVerticalControl
{
  Widget obj;
  int value;
  int prevValue;
};
typedef struct S_WidgetVerticalControl WidgetVerticalControl;

struct S_WidgetButton
{
  Widget obj;
  char text[16];
  unsigned char* pBitmapIdle;
  unsigned char* pBitmapPressed;    
};
typedef struct S_WidgetButton WidgetButton;

struct S_WidgetImage
{
  Widget obj;
  unsigned char* pBitmap;
};
typedef struct S_WidgetImage WidgetImage;

struct S_WidgetTabs
{
  Widget obj;
  int currentTabNumber;
  void (*callbackTouchEvent)(int tabNumber, int prevTabNumber);
};
typedef struct S_WidgetTabs WidgetTabs;



#define WIDGET_TYPE_VERTICAL_CONTROL  0
#define WIDGET_TYPE_BUTTON            1
#define WIDGET_TYPE_IMAGE             2
#define WIDGET_TYPE_TABS              3


#define WIDGET_STATE_IDLE     0
#define WIDGET_STATE_REDRAW   1
#define WIDGET_STATE_PRESSED  2
#define WIDGET_STATE_HIDDEN   4


void win_init(void);
void win_addWidget(int i, Widget* w);
void win_drawAll(void);
void win_touchManager(void);
void win_sysTick(void);

void win_hideWidget(Widget* w);
void win_showWidget(Widget* w);





