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

#include <chip.h>


#define LFO_WAVE_TYPE_SINE        4
#define LFO_WAVE_TYPE_TRIANGLE    3
#define LFO_WAVE_TYPE_EXP         2
#define LFO_WAVE_TYPE_SQUARE      1
#define LFO_WAVE_TYPE_RANDOM      0

void dco_init(void);



// Square settings
void dco_setPwmFrontPanelAmtForSquare(unsigned int pwmMidiValue);
void dco_setPwmAdsr2AmtForSquare(int pwmMidiValue);
void dco_setPwmLfoAmtForSquare(int pwmMidiValue);
void dco_updatePwmValueForSquare(void);
void dco_lfoSetFrontPanelPwmAndMetForSquareAndTri(int midiValue);
//________________

// Sub osc settings
void dco_setSubOctave(int flag2Octv);
void dco_setSubWaveForm(int type);
//_____________

// Ultra saw settings
void dco_updatePhaseForUltrasaw(void);
void dco_setUltraSawAmt(unsigned int midiValue);
void dco_setUltraSawRate(unsigned int midiValue);
//___________________

// Triangle settings
void dco_setMetAdsr2AmtForTriangle(int metMidiValue);
void dco_updateMetValueForTriangle(void);
void dco_setMetFrontPanelAmtForTriangle(unsigned int metMidiValue);
//___________________


// Keyboard settings
void dco_setMIDInote(int note);
void dco_setGate(unsigned char val);
void dco_setTrigger(unsigned char velocity);
//__________________


// LFO settings
void dco_lfoReset(void);
void dco_lfoFreq(unsigned int analogValue);
void dco_lfoSetWaveType(unsigned char type);
void dco_lfoSetPitch(int midiValue);
//______________




