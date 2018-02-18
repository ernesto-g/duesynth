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


void widtab_initWidget(WidgetTabs* w,int tabNumber, void (*callbackTouchEvent)(int tabNumber, int prevTabNumber) );  
void widtab_setTabNumber(WidgetTabs* w, int tabNumber);
void widtab_startDrawingTabs(WidgetTabs* w);
int widtab_runDrawingTabs(WidgetTabs* w);
int widtab_touchEvent(WidgetTabs* w,int x,int y);



