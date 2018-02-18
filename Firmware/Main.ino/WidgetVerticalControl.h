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

void widvc_startDrawingVerticalControl(WidgetVerticalControl* w);
int widvc_runDrawingVerticalControl(WidgetVerticalControl* w);
int widvc_touchEvent(WidgetVerticalControl* w, int x, int y);
int widvc_getMidiValue(WidgetVerticalControl* w);
void widvc_setMidiValue(WidgetVerticalControl* w, int midiValue);
void widvc_initWidget(WidgetVerticalControl* w,int x, int y,int value);
void widvc_clearWidgetArea(WidgetVerticalControl* w);



