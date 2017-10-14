#include <chip.h>
#include "DueTimer.h"
#include "pwm_lib.h"
using namespace arduino_due::pwm_lib;



pwm<pwm_pin::PWML0_PC2> pwm_pin34;
pwm<pwm_pin::PWML1_PC4> pwm_pin36;
pwm<pwm_pin::PWML2_PC6> pwm_pin38;
pwm<pwm_pin::PWML3_PC8> pwm_pin40;
pwm<pwm_pin::PWML4_PC21> pwm_pin9;
pwm<pwm_pin::PWML5_PC22> pwm_pin8;
pwm<pwm_pin::PWML6_PC23> pwm_pin7;
pwm<pwm_pin::PWML7_PC24> pwm_pin6;

#define PWM_MAX_VALUE 572 
#define VOICES  6
#define SAW_MAX_AMPLITUDE (PWM_MAX_VALUE/VOICES)
#define EG_MAX_AMPLITUDE  (PWM_MAX_VALUE/VOICES)

volatile unsigned int duty=0;

static volatile unsigned int squareCounters[VOICES];
static volatile unsigned int squareFreqMultiplierHalf[VOICES];
static volatile unsigned int squareFreqMultiplier[VOICES];
static volatile unsigned int eg[VOICES];

static volatile unsigned int sawCounters[VOICES];
static volatile unsigned int sawDeltas[VOICES];


void dcoUpdate(void)
{
  digitalWrite(2, HIGH);

  unsigned char i;
  signed int accSquare=0;
  signed int accSaw=0;
  signed int accTriangle=0;
  signed int accSub=0;
  
  for(i=0; i<VOICES; i++)
  {
    // squares
    squareCounters[i]++;
    if(squareCounters[i]<squareFreqMultiplierHalf[i])
    {
        accSquare-= eg[i] ;      
    }
    else if(squareCounters[i]<squareFreqMultiplier[i])
    {
        accSquare+= eg[i] ;            
    }
    else
    {
        squareCounters[i]=0;
    }

    // saws  (falta multilpicacion de amplitud)
    sawCounters[i]+=sawDeltas[i];
    accSaw+= sawCounters[i]*SAW_MAX_AMPLITUDE / 100;
    accSaw = accSaw*eg[i]/EG_MAX_AMPLITUDE;
    if(accSaw>=SAW_MAX_AMPLITUDE)
    {
        sawCounters[i]=0;
    }

    // triangle
    
    
    
  }

  accSquare+=286;
  
  pwm_pin34.set_duty_fast(accSquare);
  pwm_pin36.set_duty_fast(accSaw);
  pwm_pin38.set_duty_fast(accTriangle);
  pwm_pin34.set_duty_fast(accSub);

  digitalWrite(2, LOW);

}


void dco_init(void)
{
    //*********** PWMs ****************************
  pwm_pin34.start(681,340); // for 681 -> dutymax: 572. Freq: 146Khz
  pwm_pin36.start(681,340);
  pwm_pin38.start(681,340);
  pwm_pin40.start(681,340);

  pwm_pin7.start(681,340);
  pwm_pin6.start(681,340);
  pwm_pin8.start(681,340);
  pwm_pin9.start(681,340);

  //pwm_pin34.set_duty_fast(143); // tarda 0.7uS  572 ok

  Timer3.attachInterrupt(dcoUpdate).setFrequency(44100).start(); // freq update: 71Khz

}

