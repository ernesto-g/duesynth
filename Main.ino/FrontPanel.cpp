#include <chip.h>
#include "DcoManager.h"
#include "AnalogIns.h"
#include "WindowsManager.h"
#include "AdsrManager.h"

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
    case 0: // PulseWidth (square)
      uMidiValue = (128 * analogValue) / AN_MAX_VALUE ; // analog to midi (unsigned)
      dco_setPwmFrontPanelAmtForSquare(uMidiValue);
      dco_updatePwmValueForSquare();
      break;
    
    case 1: // ENV Amt (square)
      sMidiValue =  ((128 * analogValue) / AN_MAX_VALUE) - 64 ; // analog to midi (signed)
      adsr_setMidiPwmEnvAmtForSquare(sMidiValue);
      break;
    /*  
    case 0: // ULtraSawAmount
      dco_setUltraSawAmt(analogValue);
      break;
    case 1: // ULtraSawRate
      dco_setUltraSawRate(analogValue);
      break;
    case 4: // Metalizer
      dco_setMetalizerForTriangle(analogValue);
      break;
    case 5: // ENV Amt (triangle)
      dco_setEnvAmtForTriangle(analogValue);
      break;

    case 11: break;
    */
  }
  
}

