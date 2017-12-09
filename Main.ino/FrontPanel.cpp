#include <chip.h>
#include "AnalogIns.h"
#include "WindowsManager.h"

#define FP_STATE_IDLE           0
#define FP_STATE_GET_LCD_VALUE  1
#define FP_STATE_GET_AN_VALUE   2


static int state;
static int indexControl;

static void assignMidiValue(int indexControl,int midiValue);
static void assignAnalogValue(int indexControl,int analogValue);

void fp_init(void)
{
    state = FP_STATE_IDLE;
    indexControl=0;
}


void fp_stateMachine(void)
{
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
          if(indexControl>=12)
          {
              state = FP_STATE_IDLE;
          }
          break;
       }
    } 
}

static void assignMidiValue(int indexControl,int midiValue)
{
  switch(indexControl)
  {
      case WIN_MAN_CONTROL_ATTACK1: break;
      case WIN_MAN_CONTROL_DECAY1: break;
      case WIN_MAN_CONTROL_SUSTAIN1: break;
      case WIN_MAN_CONTROL_RELEASE1: break;
      
      case WIN_MAN_CONTROL_ATTACK2: break;
      case WIN_MAN_CONTROL_DECAY2: break;
      case WIN_MAN_CONTROL_SUSTAIN2: break;
      case WIN_MAN_CONTROL_RELEASE2: break;
  }
} 

static void assignAnalogValue(int indexControl,int analogValue)
{
  switch(indexControl)
  {
    case 0: break;
        
    case 11: break;
  }
}

