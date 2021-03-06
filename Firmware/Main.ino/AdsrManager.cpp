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
#include "pwm_lib.h"
#include "DcoManager.h"

using namespace arduino_due::pwm_lib;

#define PWM_MAX_VALUE 508  // 127*4 (a lower value is used so counters can add or substract a 4 value)

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

static int volatile attackRateCounter[ADSR_LEN];
static int volatile decayRateCounter[ADSR_LEN];
static int volatile releaseRateCounter[ADSR_LEN];

static int volatile flagEnvLowSpeed;
static int volatile lowSpeedDivider;

static volatile int pwmEnvAmtFromFrontPanel;
static volatile int metEnvAmtFromFrontPanel;

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

    attackRate[i]=64;
    decayRate[i]=64;
    sustainValue[i] = ATTACK_MAX_VALUE/2;
    releaseRate[i]=64;

    attackRateCounter[i]=attackRate[i];
    decayRateCounter[i]=decayRate[i];
    releaseRateCounter[i]=releaseRate[i];

    setAdsrPwmValue(i,adsrValue[i]);
  }

  flagEnvLowSpeed=0; // leer de entrada
  lowSpeedDivider=0;
}


void adsr_gateOnEvent(void)
{

}
void adsr_gateOffEvent(void)
{
   int i;
  for(i=0; i<ADSR_LEN; i++)
  {
      releaseRateCounter[i] = releaseRate[i];
      state[i] = STATE_RELEASE;
  }
}

void adsr_triggerEvent(int vel) // vel can be used to modulate attack rate
{
  int i;
  for(i=0; i<ADSR_LEN; i++)
  {
    /* // this way generates a clip when a second key is pressed
    if(state[i]==STATE_SUSTAIN)
    {
      // restrat adsr value
      adsrValue[i]=0;
      setAdsrPwmValue(i,adsrValue[i]);
    }
    attackRateCounter[i]=attackRate[i];
    state[i] = STATE_ATTACK;
    */
    if(state[i]!=STATE_SUSTAIN)
    {
      attackRateCounter[i]=attackRate[i];
      state[i] = STATE_ATTACK;
    }
  }
}

static void setAdsrPwmValue(int i, int value)
{
  if(value>PWM_MAX_VALUE)
    value=PWM_MAX_VALUE;
    
  if(i==0){
    pwm_pin7.set_duty_fast(value);
  }
  else {
    pwm_pin8.set_duty_fast(value);
    // update pwm env amt
    int midiVal = (value*128) / PWM_MAX_VALUE;
    if(pwmEnvAmtFromFrontPanel>=0)
    {
      dco_setPwmAdsr2AmtForSquare(  (midiVal*pwmEnvAmtFromFrontPanel)/64  ); // adsr signal positive (0 to 128)
    }
    else
    {
      dco_setPwmAdsr2AmtForSquare(  ((midiVal*pwmEnvAmtFromFrontPanel)/64) + 128 ); // adsr inverted signal (128 to 0)
    }
    dco_updatePwmValueForSquare(); // update current pwm value
    //____________________

    //update metalizer env amt
    if(metEnvAmtFromFrontPanel>=0)
    {
        dco_setMetAdsr2AmtForTriangle(  (midiVal*metEnvAmtFromFrontPanel)/64  ); // adsr signal positive (0 to 128)
    }
    else
    {
        dco_setMetAdsr2AmtForTriangle(  (midiVal*metEnvAmtFromFrontPanel)/64 + 128 ); // adsr inverted signal (128 to 0)
    }
    dco_updateMetValueForTriangle(); // update current metalizer value
    //__________________
  }
}

void adsr_stateMachineTick(void) // freq update: 14,4Khz
{
  // low speed mode (89ms to 11sec) (x10)
  if(flagEnvLowSpeed)
  {
    lowSpeedDivider++;
    if(lowSpeedDivider<10)
      return;
  }
  lowSpeedDivider=0;
  //______________________________

  
  int i;
  for(i=0; i<ADSR_LEN; i++)
  {
    switch(state[i])
    {
      case STATE_IDLE:
      {
        // idle, wait gate on, level=0
        break; 
      }
      case STATE_ATTACK:
      {
        // rising at attack rate, wait level to reach max
        attackRateCounter[i]--;
        if(attackRateCounter[i]<=0)
        {
          attackRateCounter[i]=attackRate[i];
          adsrValue[i]+=4;
          if(adsrValue[i]>=ATTACK_MAX_VALUE)
          {
            adsrValue[i] = ATTACK_MAX_VALUE;
            decayRateCounter[i] = decayRate[i];
            state[i] = STATE_DECAY;
          }
          setAdsrPwmValue(i,adsrValue[i]);
        }
        break;
      }
      case STATE_DECAY:
      {
        // falling at decay rate, wait level to reach sustain level
        decayRateCounter[i]--;
        if(decayRateCounter[i]<=0)
        {
          decayRateCounter[i] = decayRate[i];
          adsrValue[i]-=4;
          if(adsrValue[i]<=sustainValue[i])
          {
            adsrValue[i] = sustainValue[i];
            state[i] = STATE_SUSTAIN;  
          }
          setAdsrPwmValue(i,adsrValue[i]);        
        }
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
        releaseRateCounter[i]--;
        if(releaseRateCounter[i]<=0)
        {
          releaseRateCounter[i] = releaseRate[i];
          adsrValue[i]-=4;
          if(adsrValue[i]<=0)
          {  
              adsrValue[i]=0;
              state[i] = STATE_IDLE;    
          }
          setAdsrPwmValue(i,adsrValue[i]);                
        }
        break;
      }
    }
  }
}



void adsr_setMidiAttackRate(int i, int value)
{  
    attackRate[i] = value;
}
void adsr_setMidiDecayRate(int i, int value)
{
    decayRate[i] = value;
}
void adsr_setMidiReleaseRate(int i, int value)
{
    releaseRate[i] = value;
}
void adsr_setMidiSustainValue(int i, int value)
{
    sustainValue[i] = (value*SUSTAIN_MAX_VALUE)/127;
}

void adsr_setMidiPwmEnvAmtForSquare(int midiVal)
{
  pwmEnvAmtFromFrontPanel = midiVal;
}

void adsr_setMidiMetEnvAmtForTriangle(int midiVal)
{
  metEnvAmtFromFrontPanel = midiVal;
}

