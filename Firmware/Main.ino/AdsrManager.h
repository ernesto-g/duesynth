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


void adsr_stateMachineTick(void);
void adsr_init(void);

void adsr_gateOnEvent(void);
void adsr_gateOffEvent(void);
void adsr_triggerEvent(int vel);


void adsr_setMidiAttackRate(int i, int value);
void adsr_setMidiDecayRate(int i, int value);
void adsr_setMidiReleaseRate(int i, int value);
void adsr_setMidiSustainValue(int i, int value);

void adsr_setMidiPwmEnvAmtForSquare(int midiVal);
void adsr_setMidiMetEnvAmtForTriangle(int midiVal);

