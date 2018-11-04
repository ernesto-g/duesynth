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
#include "DcoManager.h"
#include "AnalogIns.h"
#include "WindowsManager.h"
#include "AdsrManager.h"
#include "Arduino.h"
#include "MIDIReception.h"
#include "Outs.h"
#include "SequencerManager.h"


#define AN_MAX_VALUE  4095

#define SET_FP_ITEM_PERIOD      10

#define FP_STATE_IDLE           0
#define FP_STATE_GET_LCD_VALUE  1
#define FP_STATE_GET_AN_VALUE   2
#define FP_STATE_READ_INS       3


#define PIN_SW_SUB_WAVEFORM 20
#define PIN_SW_SUB_OCTAVE   21
#define PIN_SW_OCTAVE_NEG   41
#define PIN_SW_SEQ_TAP_REST 42
#define PIN_SW_OCTAVE_POS   43

#define SW_STATE_IDLE         0
#define SW_STATE_WAIT         1
#define SW_STATE_WAIT_RELEASE 2
#define SW_STATE_WAIT2        3

#define TIMEOUT_IGNORE_SW     50



static int state;
static int indexControl;
static volatile int tickCounter;
static volatile int tickCounterSw;
static int swState;

static void assignMidiValue(int indexControl,int midiValue);
static void assignAnalogValue(int indexControl,int analogValue);
static unsigned char getDiscrete5ValuesFromSwitchSelector(uint16_t analogValue);
static void switchesStateMachine(void);


static const signed short ANALOG512_TO_SIGNED_MIDI_TABLE[512] PROGMEM = {-64,-64,-63,-63,-63,-63,-62,-62,-62,-61,-61,-61,-61,-60,-60,-60,-60,-59,-59,-59,-58,-58,-58,-58,-57,-57,-57,-56,-56,-56,-56,-55,-55,-55,-54,-54,-54,-54,-53,-53,-53,-53,-52,-52,-52,-51,-51,-51,-51,-50,-50,-50,-49,-49,-49,-49,-48,-48,-48,-48,-47,-47,-47,-46,-46,-46,-46,-45,-45,-45,-44,-44,-44,-44,-43,-43,-43,-42,-42,-42,-42,-41,-41,-41,-41,-40,-40,-40,-39,-39,-39,-39,-38,-38,-38,-37,-37,-37,-37,-36,-36,-36,-35,-35,-35,-35,-34,-34,-34,-34,-33,-33,-33,-32,-32,-32,-32,-31,-31,-31,-30,-30,-30,-30,-29,-29,-29,-29,-28,-28,-28,-27,-27,-27,-27,-26,-26,-26,-25,-25,-25,-25,-24,-24,-24,-23,-23,-23,-23,-22,-22,-22,-22,-21,-21,-21,-20,-20,-20,-20,-19,-19,-19,-18,-18,-18,-18,-17,-17,-17,-16,-16,-16,-16,-15,-15,-15,-15,-14,-14,-14,-13,-13,-13,-13,-12,-12,-12,-11,-11,-11,-11,-10,-10,-10,-10,-9,-9,-9,-8,-8,-8,-8,-7,-7,-7,-6,-6,-6,-6,-5,-5,-5,-4,-4,-4,-4,-3,-3,-3,-3,-2,-2,-2,-1,-1,-1,-1,-0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,3,3,3,3,4,4,4,4,5,5,5,6,6,6,6,7,7,7,8,8,8,8,9,9,9,10,10,10,10,11,11,11,11,12,12,12,13,13,13,13,14,14,14,15,15,15,15,16,16,16,16,17,17,17,18,18,18,18,19,19,19,20,20,20,20,21,21,21,22,22,22,22,23,23,23,23,24,24,24,25,25,25,25,26,26,26,27,27,27,27,28,28,28,29,29,29,29,30,30,30,30,31,31,31,32,32,32,32,33,33,33,34,34,34,34,35,35,35,35,36,36,36,37,37,37,37,38,38,38,39,39,39,39,40,40,40,41,41,41,41,42,42,42,42,43,43,43,44,44,44,44,45,45,45,46,46,46,46,47,47,47,48,48,48,48,49,49,49,49,50,50,50,51,51,51,51,52,52,52,53,53,53,53,54,54,54,54,55,55,55,56,56,56,56,57,57,57,58,58,58,58,59,59,59,60,60,60,60,61,61,61,61,62,62,62,63,63,63,63,64,64};



void fp_sysTick(void)
{
  if(tickCounter>0)
    tickCounter--;

  if(tickCounterSw>0)
    tickCounterSw--;    
}




void fp_init(void)
{
    state = FP_STATE_IDLE;
    indexControl=0;
    tickCounter=SET_FP_ITEM_PERIOD;

    swState =  SW_STATE_IDLE;

    outs_init(); 

    pinMode(PIN_SW_SUB_WAVEFORM, INPUT);
    pinMode(PIN_SW_SUB_OCTAVE, INPUT);
    
    pinMode(PIN_SW_OCTAVE_NEG, INPUT_PULLUP);
    pinMode(PIN_SW_OCTAVE_POS, INPUT_PULLUP);
    pinMode(PIN_SW_SEQ_TAP_REST, INPUT_PULLUP);

    midircv_resetOctaveOffset();
}


void fp_stateMachine(void)
{ 
  outs_stateMachine();
   
  if(tickCounter>0)
    return;

  tickCounter=SET_FP_ITEM_PERIOD;

  switchesStateMachine();
  

    
    switch(state)
    {
       case FP_STATE_IDLE:
       {
          indexControl=0;
          state = FP_STATE_GET_LCD_VALUE;
          break;
       }
       case FP_STATE_GET_LCD_VALUE:
       {
          int midiValue = wm_getControlMidiValueById(indexControl);
          assignMidiValue(indexControl,midiValue);
          indexControl++;
          if(indexControl>=8)
          {
            indexControl=0;
            state = FP_STATE_GET_AN_VALUE;
          }
          break;
       }
       case FP_STATE_GET_AN_VALUE:
       {
          uint16_t* values = ain_getValues();
          int analogValue = values[indexControl];
          assignAnalogValue(indexControl,analogValue);
          indexControl++;
          if(indexControl>=16)
          {
              state = FP_STATE_READ_INS;
          }
          break;
       }
       case FP_STATE_READ_INS:
       {            
          if(digitalRead(PIN_SW_SUB_WAVEFORM))
          {
              // triangular
              dco_setSubWaveForm(1);
          }
          else
          {
              // square
              dco_setSubWaveForm(0);
          }
          if(digitalRead(PIN_SW_SUB_OCTAVE))
              dco_setSubOctave(1); 
          else
              dco_setSubOctave(0);           
        
          state = FP_STATE_IDLE;
          break;
       }
    }

}



static void switchesStateMachine(void)
{
    static int sw;
    
    switch(swState)
    {
      case SW_STATE_IDLE:
      {
        if(digitalRead(PIN_SW_SEQ_TAP_REST)==0)
        {
          //notify to sequencer manager
          seq_tapRestEvent();
          swState = SW_STATE_WAIT;
          sw=0;
          tickCounterSw = TIMEOUT_IGNORE_SW;
        }
        else if(digitalRead(PIN_SW_OCTAVE_NEG)==0)
        {
          midircv_subOctaveOffset();
          //Serial.write("OCTAVE NEG\n");
          swState = SW_STATE_WAIT;
          sw=1;
          tickCounterSw = TIMEOUT_IGNORE_SW;
        }
        else if(digitalRead(PIN_SW_OCTAVE_POS)==0)
        {
          midircv_addOctaveOffset();
          //Serial.write("OCTAVE POS\n");
          swState = SW_STATE_WAIT;
          sw=2;
          tickCounterSw = TIMEOUT_IGNORE_SW;
        }                
        break;
      }
      case SW_STATE_WAIT:
      {
        if(tickCounterSw==0)
          swState = SW_STATE_WAIT_RELEASE;
        break;
      }
      case SW_STATE_WAIT_RELEASE:
      {
        switch(sw)
        {
          case 0:
          {
            if(digitalRead(PIN_SW_SEQ_TAP_REST)==1)
            {
                swState = SW_STATE_WAIT2;
                tickCounterSw = TIMEOUT_IGNORE_SW; 
            }
            break;
          }
          case 1:
          {
            if(digitalRead(PIN_SW_OCTAVE_NEG)==1)
            {
                swState = SW_STATE_WAIT2;
                tickCounterSw = TIMEOUT_IGNORE_SW; 
            }            
            break;
          }
          case 2:
          {
            if(digitalRead(PIN_SW_OCTAVE_POS)==1)
            {
                swState = SW_STATE_WAIT2;
                tickCounterSw = TIMEOUT_IGNORE_SW; 
            }            
            break;
          }
        }
        break;
      }
      case SW_STATE_WAIT2:
      {
        if(tickCounterSw==0)
          swState = SW_STATE_IDLE;     
        break;
      }
    }
}


static void assignMidiValue(int indexControl,int midiValue)
{
  switch(indexControl)
  {
      case WIN_MAN_CONTROL_ATTACK1: adsr_setMidiAttackRate(0,midiValue); break;
      case WIN_MAN_CONTROL_DECAY1: adsr_setMidiDecayRate(0,midiValue);break;
      case WIN_MAN_CONTROL_SUSTAIN1: adsr_setMidiSustainValue(0,midiValue);break;
      case WIN_MAN_CONTROL_RELEASE1: adsr_setMidiReleaseRate(0,midiValue); break;
      
      case WIN_MAN_CONTROL_ATTACK2: adsr_setMidiAttackRate(1,midiValue); break;
      case WIN_MAN_CONTROL_DECAY2: adsr_setMidiDecayRate(1,midiValue); break;
      case WIN_MAN_CONTROL_SUSTAIN2: adsr_setMidiSustainValue(1,midiValue); break;
      case WIN_MAN_CONTROL_RELEASE2: adsr_setMidiReleaseRate(1,midiValue);  break;
  }
} 

static void assignAnalogValue(int indexControl,int analogValue)
{
  unsigned int uMidiValue;
  int sMidiValue;  
  switch(indexControl)
  {
    case 0: // Metalizer
      uMidiValue = (128 * analogValue) / AN_MAX_VALUE ; // analog to midi (unsigned)
      dco_setMetFrontPanelAmtForTriangle(uMidiValue);
      dco_updateMetValueForTriangle(); // update current metalizer value
      break;    
    case 1: // PulseWidth (square)
      uMidiValue = (128 * analogValue) / AN_MAX_VALUE ; // analog to midi (unsigned)
      dco_setPwmFrontPanelAmtForSquare(uMidiValue);
      dco_updatePwmValueForSquare();
      break;  
    case 2: // ULtraSawAmount
      uMidiValue = (128 * analogValue) / AN_MAX_VALUE ; // analog to midi (unsigned)
      dco_setUltraSawAmt(uMidiValue);
      break;  
    case 3: // ENV Amt (triangle)
      sMidiValue =  ANALOG512_TO_SIGNED_MIDI_TABLE[analogValue>>3];  // analog to midi (signed)
      adsr_setMidiMetEnvAmtForTriangle(sMidiValue);
      break;          
    case 4: // ENV Amt (square)
      sMidiValue =  ANALOG512_TO_SIGNED_MIDI_TABLE[analogValue>>3];  // analog to midi (signed)
      adsr_setMidiPwmEnvAmtForSquare(sMidiValue);
      break;          
    case 5: // ULtraSawRate
      uMidiValue = (128 * analogValue) / AN_MAX_VALUE ; // analog to midi (unsigned)
      dco_setUltraSawRate(uMidiValue);
      break;     
    case 6: // LFO  Pitch
      sMidiValue =  ANALOG512_TO_SIGNED_MIDI_TABLE[analogValue>>3];  // analog to midi (signed)
      dco_lfoSetPitch(sMidiValue);
      break;
    case 7: // PWM and Metalizer LFO
       sMidiValue =  ANALOG512_TO_SIGNED_MIDI_TABLE[analogValue>>3];  // analog to midi (signed)
       dco_lfoSetFrontPanelPwmAndMetForSquareAndTri(sMidiValue);
      break; 
    case 8: // LFO Rate
      dco_lfoFreq(analogValue);
      break;   
    case 9: // Vibrato MOD WHEEL
      break;
    case 10: // PITCH WHEEL
      break;
    case 11: // LFO  WaveForm
      dco_lfoSetWaveType(getDiscrete5ValuesFromSwitchSelector(analogValue>>2));
      break;                 
    case 12: // Vibrato Waveform
      // 0 : rising   
      // 2020 : sine
      // 4070 : falling
      break; 
    case 13: // VIBRATO RATE 
      break; 
    case 14: // Sequencer play/stop/record
      // 0 : play
      // 2020: off
      // 4070: record
      if(analogValue<1000)
        seq_setState(SEQ_STATE_PLAY); //PLAY
      if(analogValue>=1000 && analogValue<3000)
        seq_setState(SEQ_STATE_OFF); //OFF
      if(analogValue>=3000)
        seq_setState(SEQ_STATE_RECORD); //RECORD      
      break; 
    case 15: // Sequencer rate
      int bpm = ((270*analogValue)/AN_MAX_VALUE)+30; // from 30 to 300
      seq_setBpmRate(bpm);
      break; 


  }
  
      //Serial.write("PITCH:");
      //Serial.print(sMidiValue,DEC);
      //Serial.write("\n");
            
}

static unsigned char getDiscrete5ValuesFromSwitchSelector(uint16_t analogValue)
{
    if(analogValue>=0 && analogValue<204)
      return 0;
    if(analogValue>=204 && analogValue<408)
      return 1;
    if(analogValue>=408 && analogValue<612)
      return 2;
    if(analogValue>=612 && analogValue<816)
      return 3;
    if(analogValue>=816)
      return 4;      
}


