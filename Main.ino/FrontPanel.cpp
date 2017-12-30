#include <chip.h>
#include "DcoManager.h"
#include "AnalogIns.h"
#include "WindowsManager.h"
#include "AdsrManager.h"
#include "Arduino.h"

#define AN_MAX_VALUE  4095

#define SET_FP_ITEM_PERIOD      10

#define FP_STATE_IDLE           0
#define FP_STATE_GET_LCD_VALUE  1
#define FP_STATE_GET_AN_VALUE   2


static int state;
static int indexControl;
static volatile int tickCounter;

static void assignMidiValue(int indexControl,int midiValue);
static void assignAnalogValue(int indexControl,int analogValue);
static unsigned char getDiscrete5ValuesFromSwitchSelector(uint16_t analogValue);

static const signed short ANALOG512_TO_SIGNED_MIDI_TABLE[512] PROGMEM = {-64,-64,-63,-63,-63,-63,-62,-62,-62,-61,-61,-61,-61,-60,-60,-60,-60,-59,-59,-59,-58,-58,-58,-58,-57,-57,-57,-56,-56,-56,-56,-55,-55,-55,-54,-54,-54,-54,-53,-53,-53,-53,-52,-52,-52,-51,-51,-51,-51,-50,-50,-50,-49,-49,-49,-49,-48,-48,-48,-48,-47,-47,-47,-46,-46,-46,-46,-45,-45,-45,-44,-44,-44,-44,-43,-43,-43,-42,-42,-42,-42,-41,-41,-41,-41,-40,-40,-40,-39,-39,-39,-39,-38,-38,-38,-37,-37,-37,-37,-36,-36,-36,-35,-35,-35,-35,-34,-34,-34,-34,-33,-33,-33,-32,-32,-32,-32,-31,-31,-31,-30,-30,-30,-30,-29,-29,-29,-29,-28,-28,-28,-27,-27,-27,-27,-26,-26,-26,-25,-25,-25,-25,-24,-24,-24,-23,-23,-23,-23,-22,-22,-22,-22,-21,-21,-21,-20,-20,-20,-20,-19,-19,-19,-18,-18,-18,-18,-17,-17,-17,-16,-16,-16,-16,-15,-15,-15,-15,-14,-14,-14,-13,-13,-13,-13,-12,-12,-12,-11,-11,-11,-11,-10,-10,-10,-10,-9,-9,-9,-8,-8,-8,-8,-7,-7,-7,-6,-6,-6,-6,-5,-5,-5,-4,-4,-4,-4,-3,-3,-3,-3,-2,-2,-2,-1,-1,-1,-1,-0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,3,3,3,3,4,4,4,4,5,5,5,6,6,6,6,7,7,7,8,8,8,8,9,9,9,10,10,10,10,11,11,11,11,12,12,12,13,13,13,13,14,14,14,15,15,15,15,16,16,16,16,17,17,17,18,18,18,18,19,19,19,20,20,20,20,21,21,21,22,22,22,22,23,23,23,23,24,24,24,25,25,25,25,26,26,26,27,27,27,27,28,28,28,29,29,29,29,30,30,30,30,31,31,31,32,32,32,32,33,33,33,34,34,34,34,35,35,35,35,36,36,36,37,37,37,37,38,38,38,39,39,39,39,40,40,40,41,41,41,41,42,42,42,42,43,43,43,44,44,44,44,45,45,45,46,46,46,46,47,47,47,48,48,48,48,49,49,49,49,50,50,50,51,51,51,51,52,52,52,53,53,53,53,54,54,54,54,55,55,55,56,56,56,56,57,57,57,58,58,58,58,59,59,59,60,60,60,60,61,61,61,61,62,62,62,63,63,63,63,64,64};


void fp_sysTick(void)
{
  if(tickCounter>0)
    tickCounter--;
}

void fp_init(void)
{
    state = FP_STATE_IDLE;
    indexControl=0;
    tickCounter=SET_FP_ITEM_PERIOD;
}


void fp_stateMachine(void)
{
  if(tickCounter>0)
    return;

  tickCounter=SET_FP_ITEM_PERIOD;
  
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
          if(indexControl>=12)
          {
              state = FP_STATE_IDLE;
          }
          break;
       }
    }

    // set switches states
    // void dco_setSubOctave(int flag2Octv) 
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
    /*
    // OK
    case 0: // PulseWidth (square)
      uMidiValue = (128 * analogValue) / AN_MAX_VALUE ; // analog to midi (unsigned)
      dco_setPwmFrontPanelAmtForSquare(uMidiValue);
      dco_updatePwmValueForSquare();
      break
    case 1: // ENV Amt (square)
      sMidiValue =  ANALOG512_TO_SIGNED_MIDI_TABLE[analogValue>>3];  // analog to midi (signed)
      adsr_setMidiPwmEnvAmtForSquare(sMidiValue);
      break;
    case 2: // ULtraSawAmount
      uMidiValue = (128 * analogValue) / AN_MAX_VALUE ; // analog to midi (unsigned)
      dco_setUltraSawAmt(uMidiValue);
      break;
    case 3: // ULtraSawRate
      uMidiValue = (128 * analogValue) / AN_MAX_VALUE ; // analog to midi (unsigned)
      dco_setUltraSawRate(uMidiValue);
      break;

    case 5: // ENV Amt (triangle)
      sMidiValue =  ANALOG512_TO_SIGNED_MIDI_TABLE[analogValue>>3];  // analog to midi (signed)
      adsr_setMidiMetEnvAmtForTriangle(sMidiValue);
      break;
    case 6: // LFO Rate
      dco_lfoFreq(analogValue);
      break;
    case 7: // LFO  Pitch
      sMidiValue =  ANALOG512_TO_SIGNED_MIDI_TABLE[analogValue>>3];  // analog to midi (signed)
      dco_lfoSetPitch(sMidiValue);
      break;
    case 8: // LFO  WaveForm
      dco_lfoSetWaveType(getDiscrete5ValuesFromSwitchSelector(analogValue>>2));
      break;
    case 9: // PWM and Metalizer LFO
       sMidiValue =  ANALOG512_TO_SIGNED_MIDI_TABLE[analogValue>>3];  // analog to midi (signed)
       dco_lfoSetFrontPanelPwmAndMetForSquareAndTri(sMidiValue);
      break;

    */
    case 0: // Metalizer
      uMidiValue = (128 * analogValue) / AN_MAX_VALUE ; // analog to midi (unsigned)
      dco_setMetFrontPanelAmtForTriangle(uMidiValue);
      dco_updateMetValueForTriangle(); // update current metalizer value
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


