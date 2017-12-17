#include <chip.h>
#include "pwm_lib.h"

using namespace arduino_due::pwm_lib;

#define PWM_MAX_VALUE 572
#define AN_MAX_VALUE  4095

#define ATTACK_MAX_VALUE  (PWM_MAX_VALUE) 
#define SUSTAIN_MAX_VALUE  (PWM_MAX_VALUE) 

#define ADSR_LEN      2


#define STATE_IDLE    0
#define STATE_ATTACK  1
#define STATE_DECAY   2
#define STATE_SUSTAIN 3
#define STATE_RELEASE 4



// Private variables
pwm<pwm_pin::PWML5_PC22> pwm_pin8;
pwm<pwm_pin::PWML6_PC23> pwm_pin7;
static volatile int state[ADSR_LEN];
static int volatile adsrValue[ADSR_LEN];

static int volatile attackRate[ADSR_LEN];
static int volatile decayRate[ADSR_LEN];
static int volatile sustainValue[ADSR_LEN];
static int volatile releaseRate[ADSR_LEN];


// Private functions
static void setAdsrPwmValue(int i, int value);


void adsr_init(void)
{
  pwm_pin7.start(681, 340);
  pwm_pin8.start(681, 340);   
  int i;
  for(i=0; i<ADSR_LEN; i++)
  {
    state[i]=STATE_IDLE;
    
    adsrValue[i] = 0;

    attackRate[i]=1;
    decayRate[i]=1;
    sustainValue[i] = ATTACK_MAX_VALUE/2;
    releaseRate[i]=1;
  }
}


void adsr_gateOnEvent(void)
{

}
void adsr_gateOffEvent(void)
{
   int i;
  for(i=0; i<ADSR_LEN; i++)
  {
      state[i] = STATE_RELEASE;
  }
}

void adsr_triggerEvent(int vel) // vel can be used to modulate attack rate
{
  int i;
  for(i=0; i<ADSR_LEN; i++)
  {
    if(state[i]==STATE_SUSTAIN)
    {
      // pongo a cero el valor
      adsrValue[i]=0;
      setAdsrPwmValue(i,adsrValue[i]);
    }
    state[i] = STATE_ATTACK;
  }
}

static void setAdsrPwmValue(int i, int value)
{
  if(i==0)
    pwm_pin7.set_duty_fast(value);
  else
    pwm_pin8.set_duty_fast(value);
}

void adsr_stateMachineTick(void) // freq update: 1536Hz
{
  int i;
  for(i=0; i<ADSR_LEN; i++)
  {
    switch(state[i])
    {
      case STATE_IDLE:
      {
        // idle, wait gate on, level=0
        adsrValue[i]=0;
        setAdsrPwmValue(i,adsrValue[i]);
        break; 
      }
      case STATE_ATTACK:
      {
        // rising at attack rate, wait level to reach max
        adsrValue[i]+=attackRate[i];
        if(adsrValue[i]>=ATTACK_MAX_VALUE)
        {
          adsrValue[i] = ATTACK_MAX_VALUE;
          state[i] = STATE_DECAY;
        }
        setAdsrPwmValue(i,adsrValue[i]);
        break;
      }
      case STATE_DECAY:
      {
        // falling at decay rate, wait level to reach sustain level
        adsrValue[i]-=decayRate[i];
        if(adsrValue[i]<=sustainValue[i])
        {
          adsrValue[i] = sustainValue[i];
          state[i] = STATE_SUSTAIN;  
        }
        setAdsrPwmValue(i,adsrValue[i]);        
        break;
      }
      case STATE_SUSTAIN:
      {
        // wait gate off, to go to realease state
        break;
      }
      case STATE_RELEASE:
      {
        // falling at release rate, wait level to reach zero.  
        adsrValue[i]-=releaseRate[i];
        if(adsrValue[i]<=0)
        {  
            state[i] = STATE_IDLE;    
        }
        setAdsrPwmValue(i,adsrValue[i]);                
        break;
      }
    }
  }
}



void adsr_setAnalogAttackRate(int i, int value)
{
    attackRate[i] = value;
}
void adsr_setAnalogDecayRate(int i, int value)
{
    decayRate[i] = value;
}
void adsr_setAnalogReleaseRate(int i, int value)
{
    releaseRate[i] = value;
}
void adsr_setAnalogSustainValue(int i, int value)
{
    sustainValue[i] = (value*SUSTAIN_MAX_VALUE)/AN_MAX_VALUE;
}


