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

void outs_init(void);
void outs_stateMachine(void);
void outs_set(unsigned char number,unsigned char value);
void outs_updateOuts(void);

#define OUT_LED_OCTAVE_N2 0
#define OUT_LED_OCTAVE_N1 1
#define OUT_LED_OCTAVE_0  2
#define OUT_LED_OCTAVE_P1 3
#define OUT_LED_OCTAVE_P2 4
#define OUT_LED_LFO_RATE  5
#define OUT_LED_SEQ_RATE  6






