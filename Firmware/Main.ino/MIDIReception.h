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

#include <Arduino.h>

#define MIDI_CMD_NOTE_ON  144
#define MIDI_CMD_NOTE_OFF  128


typedef struct S_MidiInfo {
  byte channel;
  byte cmd;
  byte note;
  byte vel;
}MidiInfo;


void midircv_init(void);
void midircv_stateMachine(void);
void midircv_sysTick(void);

void midircv_resetOctaveOffset(void);
void midircv_addOctaveOffset(void);
void midircv_subOctaveOffset(void);


