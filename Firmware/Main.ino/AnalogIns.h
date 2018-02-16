/**
 *  Arduino Due Synth
    Copyright (C) <2017>  Ernesto Gigliotti <ernestogigliotti@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#define ANALOG_STATE_IDLE   0
#define ANALOG_STATE_START  1
#define ANALOG_STATE_WAIT   2
#define ANALOG_STATE_FINISH 3
#define ANALOG_STATE_WAIT_CHN_MUX 4
#define ANALOG_STATE_WAIT_CHN_MUX2 5

void ain_init(void);
void ain_state_machine(void);
uint16_t* ain_getValues(void);
uint8_t ain_isReady(void);

