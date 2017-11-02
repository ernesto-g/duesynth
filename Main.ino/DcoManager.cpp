#include <chip.h>
#include "DueTimer.h"
#include "pwm_lib.h"
#include "DcoManager.h"
#include "WaveTables.h"
using namespace arduino_due::pwm_lib;

// PWM pins
pwm<pwm_pin::PWML0_PC2> pwm_pin34;
pwm<pwm_pin::PWML1_PC4> pwm_pin36;
pwm<pwm_pin::PWML2_PC6> pwm_pin38;
pwm<pwm_pin::PWML3_PC8> pwm_pin40;
pwm<pwm_pin::PWML4_PC21> pwm_pin9;
pwm<pwm_pin::PWML5_PC22> pwm_pin8;
pwm<pwm_pin::PWML6_PC23> pwm_pin7;
pwm<pwm_pin::PWML7_PC24> pwm_pin6;

// defines
#define PWM_MAX_VALUE 572
#define SAW_MAX_AMPLITUDE (PWM_MAX_VALUE)
#define EG_MAX_AMPLITUDE  (PWM_MAX_VALUE)

#define SQUARE_COUNTERS 2 // square and sub
#define EG_COUNTERS     2 // adsr 1 and adsr 2
#define SAW_COUNTERS    3 // for ultrasaw
#define TRIANGLE_COUNTERS 1
#define METALIZER_STAGES  3

#define ADSR1     0
#define ADSR2     1

#define AN_MAX_VALUE  4095





// private variables
static volatile unsigned int squareCounters[SQUARE_COUNTERS];
static volatile unsigned int squareFreqMultiplierHalf[SQUARE_COUNTERS];
static volatile unsigned int squareFreqMultiplier[SQUARE_COUNTERS];
static volatile unsigned int pwmForSquareValue;


//static volatile unsigned int eg[EG_COUNTERS];

static volatile signed int sawCounters[SAW_COUNTERS];
static volatile int flagSawPhaseChanged;
static volatile unsigned int ultrasawForSawValue;
static volatile int phase1Value;
static volatile int phase2Value;

static volatile signed int triangleCounters[TRIANGLE_COUNTERS];
static volatile signed int triangleDelta[TRIANGLE_COUNTERS];
static volatile signed int metalizerLevel[METALIZER_STAGES];

static volatile int lfoCounter;
static volatile int lfoWaveType;
static volatile int lfoFreqMultiplier;




// freq tables
static unsigned int TABLE_SQUARE_FREQ[] = {2618, 2471, 2333, 2202, 2078, 1961, 1851, 1747, 1649, 1557, 1469, 1387, 1309, 1236, 1166, 1101, 1039, 981, 926, 874, 825, 778, 735, 693, 655, 618, 583, 550, 520, 490, 463, 437, 412, 389, 367, 347, 327, 309, 292, 275, 260, 245, 231, 218, 206, 195, 184, 173, 164, 154, 146, 138, 130, 123, 116, 109, 103, 97, 92, 87, 82, 77, 73, 69, 65, 61, 58, 55, 52, 49, 46, 43, 41, 39, 36, 34};

#include "WaveTables.cpp"


void dcoUpdateMono(void)
{
  //digitalWrite(2, HIGH);

  unsigned char i;
  signed int accSquare = 0;
  signed int accSaw = 0;
  signed int accTri = 0;
  signed int accSub = 0;

  // square
  squareCounters[0]++;
  if (squareCounters[0] < squareFreqMultiplierHalf[0])
  {
    accSquare -= (PWM_MAX_VALUE / 2) ;
  }
  else if (squareCounters[0] < squareFreqMultiplier[0])
  {
    accSquare += (PWM_MAX_VALUE / 2) ;
  }
  else
  {
    squareCounters[0] = 0;
  }

  // sub (square)
  squareCounters[1]++;
  if (squareCounters[1] < squareFreqMultiplierHalf[1])
  {
    accSub -= (PWM_MAX_VALUE / 2) ;
  }
  else if (squareCounters[1] < squareFreqMultiplier[1])
  {
    accSub += (PWM_MAX_VALUE / 2) ;
  }
  else
  {
    squareCounters[1] = 0;
  }

  // saw
  for (i = 0; i < 3; i++)
  {
    sawCounters[i]++;
    if (sawCounters[i] >= 0)
    {
      if (sawCounters[i] < squareFreqMultiplier[0])
      {
        accSaw += ((PWM_MAX_VALUE / 3) * sawCounters[i]) / squareFreqMultiplier[0];
      }
      else
      {
        if (flagSawPhaseChanged == 1 && i == 2)
        {
          flagSawPhaseChanged = 0;
          // reoad counters with new phase values
          sawCounters[0] = 0;
          sawCounters[1] = phase1Value; // -1*(squareFreqMultiplier[0]/4);
          sawCounters[2] = phase2Value; //-1*(squareFreqMultiplier[0]/2);
        }
        else
          sawCounters[i] = 0;

      }
    }

  }

  // triangle
  triangleCounters[0]++;
  if (triangleDelta[0] == 1)
  {
    // positive
    accTri = ((2 * PWM_MAX_VALUE) * triangleCounters[0]) / (squareFreqMultiplier[0]);
    if (triangleCounters[0] < (squareFreqMultiplier[0] >> 1))
    {
    }
    else
    {
      triangleDelta[0] = 0;
    }
  }
  else
  {
    // negative
    accTri = (2 * PWM_MAX_VALUE) - ((2 * PWM_MAX_VALUE) * triangleCounters[0]) / (squareFreqMultiplier[0]);
    if (triangleCounters[0] < squareFreqMultiplier[0]) // paso la mitad
    {
    }
    else
    {
      triangleDelta[0] = 1;
      triangleCounters[0] = 0;
    }
  }
  //__________



  for (i = 0; i < METALIZER_STAGES ; i++)
  {
    if (accTri > (PWM_MAX_VALUE - metalizerLevel[i]) )
    {
      accTri = (2 * (PWM_MAX_VALUE - metalizerLevel[i])) - accTri;
    }
    else if (accTri < metalizerLevel[i] )
    {
      accTri = (2 * metalizerLevel[i]) - accTri;
    }
  }

  accSquare += (PWM_MAX_VALUE / 2);
  accSub += (PWM_MAX_VALUE / 2);

  pwm_pin34.set_duty_fast(accSquare);
  pwm_pin36.set_duty_fast(accSaw);
  pwm_pin40.set_duty_fast(accSub);
  pwm_pin38.set_duty_fast(accTri);

  //digitalWrite(2, LOW);

}



static void dcoUpdateLFO(void)
{
  lfoCounter += lfoFreqMultiplier;
  if (lfoCounter >= LFO_TABLE_SIZE) {
    lfoCounter = lfoCounter - LFO_TABLE_SIZE;
  }

  switch (lfoWaveType)
  {
    case LFO_WAVE_TYPE_SINE:
      pwm_pin6.set_duty_fast(SINETABLE[lfoCounter]);
      break;

    case LFO_WAVE_TYPE_TRIANGLE:
      //OCR2B = pgm_read_byte_near(TRIANGLETABLE + lfoCounter );
      pwm_pin6.set_duty_fast(TRIANGLETABLE[lfoCounter]);
      break;
    case LFO_WAVE_TYPE_EXP:
      //OCR2B = pgm_read_byte_near(EXPTABLE + lfoCounter );
      pwm_pin6.set_duty_fast(EXPTABLE[lfoCounter]);
      break;
    case LFO_WAVE_TYPE_SQUARE:
      if (lfoCounter < (LFO_TABLE_SIZE / 2))
        //  OCR2B = 255;
        pwm_pin6.set_duty_fast(572);
      else
        //  OCR2B = 0;
        pwm_pin6.set_duty_fast(0);
      break;

  }
}

void dco_init(void)
{
  //*********** PWMs ****************************
  pwm_pin34.start(681, 340); // for 681 -> dutymax: 572. Freq: 146Khz
  pwm_pin36.start(681, 340);
  pwm_pin38.start(681, 340);
  pwm_pin40.start(681, 340);

  pwm_pin7.start(681, 340);
  pwm_pin6.start(681, 340);
  pwm_pin8.start(681, 340);
  pwm_pin9.start(681, 340);

  //pwm_pin34.set_duty_fast(143); // tarda 0.7uS  572 ok
  triangleDelta[0] = 1;
  triangleCounters[0] = 0;

  metalizerLevel[0] = 250;
  metalizerLevel[1] = 180;
  metalizerLevel[2] = 120;

  lfoWaveType = LFO_WAVE_TYPE_SINE;
  lfoCounter = 0;
  lfoFreqMultiplier = 30; // from 1 to 60 for 0.5Hz to 30Hz


  Timer3.attachInterrupt(dcoUpdateMono).setFrequency(72000).start(); // freq update: 72Khz
  Timer4.attachInterrupt(dcoUpdateLFO).setFrequency(1536).start(); // freq update: 1536Hz

  flagSawPhaseChanged = 0;

  //debug
 // eg[0] = (PWM_MAX_VALUE / 2);
  pwmForSquareValue = 0;
  ultrasawForSawValue = 0;
  dco_setMIDInote(45);
  //______

}

void dco_setPwmForSquare(unsigned int analogValue)
{
  unsigned int pwmPercentValue = 100 - (  (((90 - 50) * analogValue) / AN_MAX_VALUE) + 50  ) ;
  squareFreqMultiplierHalf[0] = (squareFreqMultiplier[0] * pwmPercentValue) / 100;

  pwmForSquareValue = analogValue;
}

void dco_setPhaseForUltrasaw(unsigned int analogValue)
{
  unsigned int percentValue =    (90 * analogValue) / AN_MAX_VALUE   ;

  phase2Value = -1 * ((squareFreqMultiplier[0] * percentValue) / 100);
  phase1Value = -1 * ((squareFreqMultiplier[0] * (percentValue >> 1)) / 100);

  ultrasawForSawValue = analogValue;
  flagSawPhaseChanged = 1;
}

void dco_setMetalizerForTriangle(unsigned int analogValue)
{
    if(analogValue<(AN_MAX_VALUE/3))
    {
      metalizerLevel[0] = (MET_0_MAX*analogValue)/(AN_MAX_VALUE/3);
      metalizerLevel[1] = 0;
      metalizerLevel[2] = 0;
    }
    else if(analogValue<(AN_MAX_VALUE*2/3))
    {
      metalizerLevel[0] = MET_0_MAX;
      metalizerLevel[1] = hacer cuenta;
      metalizerLevel[2] = 0;      
    }
    else
    {
      metalizerLevel[0] = MET_0_MAX;
      metalizerLevel[1] = MET_1_MAX;
      metalizerLevel[2] = hacer cuenta;            
    }
}

void dco_setMIDInote(int note)
{
  if (note >= 21 && note <= 96)
  {
    note = note - 21;
    // set square
    signed int f = (signed int)TABLE_SQUARE_FREQ[note];
    squareFreqMultiplier[0] = f;
    // check pwm for set squareFreqMultiplierHalf
    dco_setPwmForSquare(pwmForSquareValue);
    //____________

    // set saw
    dco_setPhaseForUltrasaw(ultrasawForSawValue);


    // set sub
    signed int noteSub = note - 12;
    if (noteSub >= 0)
    {
      f = TABLE_SQUARE_FREQ[noteSub];
      squareFreqMultiplier[1] = f;
      squareFreqMultiplierHalf[1] = f >> 1;
    }
    //_________


  }

}

void dco_setGate(byte val)
{
  if (val == 1)
  {
    // gate ON
  }
  else
  {
    // gate OFF
  }
}

void dco_setTrigger(byte velocity)// 0...127
{

}

void dco_lfoReset(void)
{
  lfoCounter = 0;
}

void dco_lfoFreq(byte value) // 1 to 60 (0.5hz to 30hz)
{
  lfoFreqMultiplier = value;
}

void dco_lfoSetWaveType(byte type)
{
    lfoWaveType=type;
}

