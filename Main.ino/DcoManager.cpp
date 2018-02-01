#include <chip.h>
#include "DueTimer.h"
#include "pwm_lib.h"
#include "DcoManager.h"
#include "WaveTables.h"
#include "AdsrManager.h"
#include "Outs.h"
using namespace arduino_due::pwm_lib;

// PWM pins
pwm<pwm_pin::PWML0_PC2> pwm_pin34;
pwm<pwm_pin::PWML1_PC4> pwm_pin36;
pwm<pwm_pin::PWML2_PC6> pwm_pin38;
pwm<pwm_pin::PWML3_PC8> pwm_pin40;
pwm<pwm_pin::PWML4_PC21> pwm_pin9;

pwm<pwm_pin::PWML7_PC24> pwm_pin6;

// defines
#define PWM_MAX_VALUE 572
#define SAW_MAX_AMPLITUDE (PWM_MAX_VALUE)
#define EG_MAX_AMPLITUDE  (PWM_MAX_VALUE)

#define SAW_COUNTERS    3 // for ultrasaw

#define ADSR1     0
#define ADSR2     1

#define AN_MAX_VALUE  4095


// private variables
static volatile unsigned int currentNoteCounterValue;
static volatile unsigned int currentNoteCounterValueHalf;
static volatile unsigned int currentNoteCounterValueOriginal;

static volatile unsigned int squareCounter;
static volatile int pwmAdsr2Amt;
static volatile int pwmLfoAmt;
static volatile unsigned int pwmFrontPanelAmt;
static volatile int pwmAndMetFrontPanelAmt;


static volatile signed int triangleCounter;
static volatile signed int triangleDelta;
static volatile int metAdsr2Amt;
static volatile unsigned int metFrontPanelAmt;
static volatile int metalizerMaxAmplitudeMidi;
static volatile int counterMet=0;
static volatile int metValueToSub=0;
static volatile int metalizerMaxTimeGap;


static volatile signed int sawCounters[SAW_COUNTERS];
static volatile int flagSawPhaseChanged;
static volatile unsigned int ultrasawForSawValue;
static volatile int phase1Value;
static volatile int phase2Value;
static volatile int ultraSawAmt;
static volatile int ultraSawPhase0InPwmScale;
static volatile int ultraSawPhase1InPwmScale;


static volatile unsigned int squareCounterSub;
static volatile int flagSubOctave;


static volatile int lfoCounter;
static volatile int lfoWaveType;
static volatile int lfoFreqMultiplier;
static volatile int lfoSecondaryDivider;
static volatile int lfoUltraSaw0Counter;
static volatile int lfoUltraSaw1Counter;
static volatile int lfoUltraSaw1Multiplier;
static volatile unsigned short randomCounter=0;
static volatile int lfoSampleAndHoldNewSampleFlag=0;
static volatile int lfoSampleAndHoldValue=0;
static volatile int pitchLfoFrontPanelAmt=0;

#define INC_RANDOM_COUNTER()  {randomCounter++; if(randomCounter>=(RANDOM_TABLE_SIZE-1)) randomCounter=0;}

static volatile int adsrDivider=0;


// freq tables
static unsigned int TABLE_SQUARE_FREQ[] = {2618, 2471, 2333, 2202, 2078, 1961, 1851, 1747, 1649, 1557, 1469, 1387, 1309, 1236, 1166, 1101, 1039, 981, 926, 874, 825, 778, 735, 693, 655, 618, 583, 550, 520, 490, 463, 437, 412, 389, 367, 347, 327, 309, 292, 275, 260, 245, 231, 218, 206, 195, 184, 173, 164, 154, 146, 138, 130, 123, 116, 109, 103, 97, 92, 87, 82, 77, 73, 69, 65, 61, 58, 55, 52, 49, 46, 43, 41, 39, 36, 34};
#include "WaveTables.cpp"


void dcoUpdateMono(void)
{
  //digitalWrite(21, HIGH);

  unsigned char i;
  signed int accSquare = 0;
  signed int accSaw = 0;
  signed int accTri = 0;
  signed int accSub = 0;

  // square
  squareCounter++;
  if (squareCounter < currentNoteCounterValueHalf)
  {
    accSquare -= (PWM_MAX_VALUE / 2) ;
  }
  else if (squareCounter < currentNoteCounterValue)
  {
    accSquare += (PWM_MAX_VALUE / 2) ;
  }
  else
  {
    squareCounter = 0;
  }

  // sub (square)
  unsigned int currentNoteCounterValueHalfSub;
  unsigned int currentNoteCounterValueSub;
  if(flagSubOctave)
  {
    currentNoteCounterValueHalfSub = currentNoteCounterValueHalf*4;
    currentNoteCounterValueSub = currentNoteCounterValue*4;
  }
  else
  {
    currentNoteCounterValueHalfSub = currentNoteCounterValueHalf*2;
    currentNoteCounterValueSub = currentNoteCounterValue*2;    
  }
  squareCounterSub++;
  if (squareCounterSub < currentNoteCounterValueHalfSub)
  {
    accSub -= (PWM_MAX_VALUE / 2) ;
  }
  else if (squareCounterSub < currentNoteCounterValueSub)
  {
    accSub += (PWM_MAX_VALUE / 2) ;
  }
  else
  {
    squareCounterSub = 0;
  }

  // saw
  for (i = 0; i < 3; i++)
  {
    sawCounters[i]++;
    if (sawCounters[i] >= 0)
    {
      if (sawCounters[i] < currentNoteCounterValue)
      {
        accSaw += ((PWM_MAX_VALUE / 3) * sawCounters[i]) / currentNoteCounterValue;
      }
      else
      {
        if (flagSawPhaseChanged == 1 && i == 2)
        {
          flagSawPhaseChanged = 0;
          // reoad counters with new phase values
          sawCounters[0] = 0;
          sawCounters[1] = phase1Value; 
          sawCounters[2] = phase2Value; 
        }
        else
          sawCounters[i] = 0;

      }
    }

  }

  // triangle
  triangleCounter++;
  if (triangleDelta == 1)
  {
    // positive
    accTri = ((2 * PWM_MAX_VALUE) * triangleCounter) / (currentNoteCounterValue);
    if (triangleCounter < (currentNoteCounterValue >> 1))
    {
    }
    else
    {
      triangleDelta = 0;
    }
  }
  else
  {
    // negative
    accTri = (2 * PWM_MAX_VALUE) - ((2 * PWM_MAX_VALUE) * triangleCounter) / (currentNoteCounterValue);
    if (triangleCounter < currentNoteCounterValue) // paso la mitad
    {
    }
    else
    {
      triangleDelta = 1;
      triangleCounter = 0;
    }
  }
  
  // More if cases are commented due performance
  unsigned int currentNoteCounterValue_1_16 = (currentNoteCounterValue >> 4);
  //unsigned int currentNoteCounterValue_1_8 = (currentNoteCounterValue >> 3);
  //unsigned int currentNoteCounterValue_1_4 = (currentNoteCounterValue >> 2);
  unsigned int currentNoteCounterValue_1_2 = (currentNoteCounterValue >> 1);
  /*
  if (triangleCounter == currentNoteCounterValue_1_16 ) // 1/16
  {
      counterMet=metalizerMaxTimeGap;
      metValueToSub = -((PWM_MAX_VALUE/2) - accTri);
  }
  else if (triangleCounter == currentNoteCounterValue_1_8 ) // 1/8
  {
      counterMet=metalizerMaxTimeGap;
      metValueToSub = -((PWM_MAX_VALUE/2) - accTri);
  }
  
  if (triangleCounter == (currentNoteCounterValue_1_16 + currentNoteCounterValue_1_4 ) ) // 1/16 + 1/4
  {
      counterMet=metalizerMaxTimeGap;
      metValueToSub = accTri - (PWM_MAX_VALUE/2);
  }
  else if (triangleCounter == (currentNoteCounterValue_1_8 + currentNoteCounterValue_1_4 ) ) // 1/8 + 1/4
  {
      counterMet=metalizerMaxTimeGap;
      metValueToSub = accTri - (PWM_MAX_VALUE/2);
  }
  */
  if (triangleCounter == ( currentNoteCounterValue_1_2 ) - currentNoteCounterValue_1_16 ) // 1/2 - 1/16
  {
      counterMet=metalizerMaxTimeGap;
      metValueToSub = accTri - (PWM_MAX_VALUE/2);
  }  
  else if (triangleCounter == ( currentNoteCounterValue_1_2 ) + currentNoteCounterValue_1_16 ) // 1/2 + 1/16
  {
      counterMet=metalizerMaxTimeGap;
      metValueToSub = accTri - (PWM_MAX_VALUE/2);
  }   
  /*
  else if (triangleCounter == ( currentNoteCounterValue_1_2 ) + currentNoteCounterValue_1_4 + currentNoteCounterValue_1_16 ) // 1/2 + 1/4 + 1/16 
  {
      counterMet=metalizerMaxTimeGap;
      metValueToSub = -((PWM_MAX_VALUE/2) - accTri);
  } 
  else if (triangleCounter == ( currentNoteCounterValue_1_2 ) + currentNoteCounterValue_1_4 + currentNoteCounterValue_1_8 ) // 1/2 + 1/4 + 1/8 
  {
      counterMet=metalizerMaxTimeGap;
      metValueToSub = -((PWM_MAX_VALUE/2) - accTri);
  }    
  */
  if(counterMet>0)
  {
      counterMet--;
      accTri = accTri - ((metValueToSub*metalizerMaxAmplitudeMidi)/128);    
      if(accTri<0)
        accTri=0;
  }
  
  //_________________________________________________________________________________________________________________  
  






  accSquare += (PWM_MAX_VALUE / 2);
  accSub += (PWM_MAX_VALUE / 2);

  // secure limits
  if(accTri>PWM_MAX_VALUE)
    accTri=PWM_MAX_VALUE;
  if(accSaw>PWM_MAX_VALUE)
    accSaw=PWM_MAX_VALUE;
  if(accSquare>PWM_MAX_VALUE)
    accSquare=PWM_MAX_VALUE;
  if(accSub>PWM_MAX_VALUE)
    accSub=PWM_MAX_VALUE;
  //______________

  pwm_pin34.set_duty_fast(accSquare);
  pwm_pin36.set_duty_fast(accSaw);
  pwm_pin40.set_duty_fast(accSub);
  pwm_pin38.set_duty_fast(accTri);


  adsrDivider++;
  if(adsrDivider>=5)
  {
    adsrDivider=0;
    adsr_stateMachineTick(); //14,4Khz
  }

  INC_RANDOM_COUNTER();

  //digitalWrite(21, LOW);

}


static volatile byte lfoRateLed=0;

static void dcoUpdateLFO(void)
{
  
  lfoCounter += lfoFreqMultiplier;
  if (lfoCounter >= LFO_TABLE_SIZE) {
    lfoCounter = lfoCounter - LFO_TABLE_SIZE;
    lfoSampleAndHoldNewSampleFlag = 1;

    lfoRateLed = (~lfoRateLed);
    if(lfoRateLed)
      outs_set(OUT_LED_LFO_RATE,1);
    else
      outs_set(OUT_LED_LFO_RATE,0);        
  }

  int val;
  switch (lfoWaveType)
  {
    case LFO_WAVE_TYPE_SINE:
      val = SINETABLE[lfoCounter];
      break;
    case LFO_WAVE_TYPE_TRIANGLE:
      val = TRIANGLETABLE[lfoCounter] ;
      break;
    case LFO_WAVE_TYPE_EXP:
      val = EXPTABLE[lfoCounter] ;
      break;
    case LFO_WAVE_TYPE_SQUARE:
      if (lfoCounter < (LFO_TABLE_SIZE / 2))
        val = PWM_MAX_VALUE;
      else
        val = 0;
      break;
    case LFO_WAVE_TYPE_RANDOM:
      if(lfoSampleAndHoldNewSampleFlag==1) // sample new value
      {
        lfoSampleAndHoldValue = RANDOMTABLE[randomCounter]; // hold value
        lfoSampleAndHoldNewSampleFlag = 0;
      } 
      val = lfoSampleAndHoldValue ; 
      break;
      
  }

  if(val>PWM_MAX_VALUE)
    val = PWM_MAX_VALUE;
  pwm_pin6.set_duty_fast(val);

  int midiVal = (val*128) / PWM_MAX_VALUE;
  val = val - (PWM_MAX_VALUE/2); // convert to signed value

  // pitch modulation
  int modVal;
  if(pitchLfoFrontPanelAmt>=0)
  {
      modVal = (val*pitchLfoFrontPanelAmt)/64; // pitch signal positive (-(PWM_MAX_VALUE/2) to +(PWM_MAX_VALUE/2))
  }
  else
  {
      modVal = (( (0)-val)*pitchLfoFrontPanelAmt)/64; // pitch inverted signal (-(PWM_MAX_VALUE/2) to +(PWM_MAX_VALUE/2))   
  }
  int delta = ((currentNoteCounterValueOriginal*2)-currentNoteCounterValueOriginal);
  currentNoteCounterValue = currentNoteCounterValueOriginal +  ( (delta * modVal)/PWM_MAX_VALUE );
  //___________________

  // update pwm lfo amt
  if(pwmAndMetFrontPanelAmt>=0)
  {
    dco_setPwmLfoAmtForSquare(  (midiVal*pwmAndMetFrontPanelAmt)/64  ); // adsr signal positive (0 to 128)
  }
  else
  {
    dco_setPwmLfoAmtForSquare(  ((midiVal*pwmAndMetFrontPanelAmt)/64) + 128 ); // adsr inverted signal (128 to 0)
  }
  
  dco_updatePwmValueForSquare(); // update current pwm value
  dco_updateMetValueForTriangle(); // update current metalizer value 
  dco_updatePhaseForUltrasaw(); // update current phases value
  //_____________________

  // Secondary LFOs ****************************
  lfoSecondaryDivider++;
  if(lfoSecondaryDivider>=3) // 0.16 to 10Hz
  {
    lfoSecondaryDivider=0;
    
    // ultrasaw LFO
    lfoUltraSaw0Counter += 6; // for 1Hz
    if (lfoUltraSaw0Counter >= LFO_TABLE_SIZE) {
      lfoUltraSaw0Counter = lfoUltraSaw0Counter - LFO_TABLE_SIZE;
    }
    lfoUltraSaw1Counter += lfoUltraSaw1Multiplier;
    if (lfoUltraSaw1Counter >= LFO_TABLE_SIZE) {
      lfoUltraSaw1Counter = lfoUltraSaw1Counter - LFO_TABLE_SIZE;
    }
    ultraSawPhase0InPwmScale = SINETABLE[lfoUltraSaw0Counter];
    ultraSawPhase1InPwmScale = SINETABLE[lfoUltraSaw1Counter];  
    dco_updatePhaseForUltrasaw();
    //_____________
  
    // Vibrato LFO
    //______________
  }
  //____________________________________________
    
}




void dco_init(void)
{
  //*********** PWMs ****************************
  pwm_pin34.start(681, 340); // for 681 -> dutymax: 572. Freq: 146Khz
  pwm_pin36.start(681, 340);
  pwm_pin38.start(681, 340);
  pwm_pin40.start(681, 340);
  pwm_pin6.start(681, 340);
  pwm_pin9.start(681, 340);

  triangleDelta = 1;
  triangleCounter = 0;

  lfoWaveType = LFO_WAVE_TYPE_SINE;
  lfoCounter = 0;
  lfoFreqMultiplier = 10; // from 1 to 180 for 0.5Hz to 90Hz
  lfoSecondaryDivider = 0;

  lfoUltraSaw0Counter=0;
  lfoUltraSaw1Counter=0;
  lfoUltraSaw1Multiplier=10;
  ultraSawPhase0InPwmScale=0;
  ultraSawPhase1InPwmScale=0;


  Timer3.attachInterrupt(dcoUpdateMono).setFrequency(72000).start(); // freq update: 72Khz
  Timer4.attachInterrupt(dcoUpdateLFO).setFrequency(1536).start(); // freq update: 1536Hz

  flagSawPhaseChanged = 0;
  flagSubOctave=1;

  //debug

  pwmAdsr2Amt = 0;
  pwmLfoAmt = 0;
  pwmFrontPanelAmt = 0;

  metAdsr2Amt = 0;
  metFrontPanelAmt = 0;
    
  ultrasawForSawValue = 0;
  dco_setMIDInote(45);
  //______

}

// Square PWM management ****************************************************************************
void dco_updatePwmValueForSquare(void)
{
    int acc = pwmFrontPanelAmt + pwmLfoAmt + pwmAdsr2Amt;

    acc = 64 + (acc*51)/128; // output: 64 to 115 (50% -> 90% parts of 128)

    if(acc>115) // modulation clipping
      acc = 115; // 115 = 90%
    if(acc<64)
      acc = 64; // 64 = 50%
      
    currentNoteCounterValueHalf = (currentNoteCounterValue * acc) / 128;
}
void dco_setPwmAdsr2AmtForSquare(int pwmMidiValue) // used by adsr2
{
    pwmAdsr2Amt = pwmMidiValue;
}
void dco_setPwmLfoAmtForSquare(int pwmMidiValue) // used by lfo
{
    pwmLfoAmt = pwmMidiValue; // it is used for metalizer too
}
void dco_setPwmFrontPanelAmtForSquare(unsigned int pwmMidiValue) // used by front panel pot
{
    pwmFrontPanelAmt = pwmMidiValue;
}

//____________________________________________________________________________________________________

// TRIANGLE management ************************************************************************************
void dco_updateMetValueForTriangle(void)
{
    int acc = metFrontPanelAmt + pwmLfoAmt + metAdsr2Amt; // LFO amt its the same for PWM and METALIZER

    if(acc>128) // modulation clipping
      acc = 128; // 100%
    if(acc<0)
      acc = 0;  // 0%

    metalizerMaxAmplitudeMidi = acc;  
    metalizerMaxTimeGap = ( (currentNoteCounterValue * acc) / 128) / 16 ;  
}

void dco_setMetAdsr2AmtForTriangle(int metMidiValue) // used by adsr2
{
    metAdsr2Amt = metMidiValue;
}
void dco_setMetFrontPanelAmtForTriangle(unsigned int metMidiValue) // used by front panel pot
{
   metFrontPanelAmt = metMidiValue;
}
//____________________________________________________________________________________________________







// SAW management ************************************************************************************
void dco_updatePhaseForUltrasaw(void)
{
  int phase0;
  int phase1;

  phase0 = (ultraSawPhase0InPwmScale * ultraSawAmt) / 128;
  phase1 = (ultraSawPhase1InPwmScale * ultraSawAmt) / 128;
  
  phase2Value = -1 * ((currentNoteCounterValue * phase1) / PWM_MAX_VALUE);
  phase1Value = -1 * ((currentNoteCounterValue * phase0) / PWM_MAX_VALUE);
  flagSawPhaseChanged = 1;
}
void dco_setUltraSawAmt(unsigned int midiValue)
{
    ultraSawAmt = (signed)midiValue;
}
void dco_setUltraSawRate(unsigned int midiValue)
{
    lfoUltraSaw1Multiplier = ((midiValue * 60) / 128 ) + 1 ; // from 1 to 60; //  0.16Hz to 10Hz
}
//____________________________________________________________________________________________________


// SUB management ************************************************************************************
void dco_setSubOctave(int flag2Octv)
{
  flagSubOctave = flag2Octv;
}
void dco_updateNoteForSubOsc(void)
{
  // sub osc note is calculated in timer interrupt
}
//____________________________________________________________________________________________________


void dco_setMIDInote(int note)
{
  if (note >= 21 && note <= 96)
  {
    note = note - 21;
    signed int f = (signed int)TABLE_SQUARE_FREQ[note];
    currentNoteCounterValueOriginal = f;
    currentNoteCounterValue = currentNoteCounterValueOriginal;

    // set square
    dco_updatePwmValueForSquare(); // update current pwm value for new freq note

    // set saw
    dco_updatePhaseForUltrasaw(); // update current pwm value

    // set triangle
    dco_updateMetValueForTriangle(); // update current metalizer value 
    
    // set sub
    dco_updateNoteForSubOsc(); // update current note for sub oscillator
  }

  INC_RANDOM_COUNTER();

}

void dco_setGate(byte val)
{
  if (val == 1)
  {
    // gate ON
    adsr_gateOnEvent();
    //...
  }
  else
  {
    // gate OFF
    adsr_gateOffEvent();
    //...    
  }
}

void dco_setTrigger(unsigned char velocity)// 0...127
{
    adsr_triggerEvent(velocity);
    //...
}

// LFO management **********************************************************************************
void dco_lfoReset(void)
{
  lfoCounter = 0;
}
void dco_lfoFreq(unsigned int analogValue) // 1 to 180 (0.5hz to 90hz)
{
  lfoFreqMultiplier = (analogValue*179 / AN_MAX_VALUE) + 1 ;
}
void dco_lfoSetWaveType(unsigned char type)
{
    lfoWaveType=type;
}
void dco_lfoSetFrontPanelPwmAndMetForSquareAndTri(int midiValue)
{
    pwmAndMetFrontPanelAmt = midiValue;
}
void dco_lfoSetPitch(int midiValue)
{
  pitchLfoFrontPanelAmt = midiValue;
}
//____________________________________________________________________________________________________

