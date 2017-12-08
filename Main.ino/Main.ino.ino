#include <chip.h>
#include "DueTimer.h"
#include "pwm_lib.h"
#include "DcoManager.h"
#include "AnalogIns.h"
#include "MIDIReception.h"
#include "MIDIManager.h"

// LCD and Touch libraries
#include "UTFT/UTFT.h"
#include "Fonts.h"
#include "Images.h"
#include "URTouch/URTouch.h"
#include "LCDManager.h"
UTFT myGLCD(ILI9325D_16,30,31,32,33);
URTouch  myTouch( 10, 5, 4, 3, 2);
//_________________________

#include "WindowsManager.h"

using namespace arduino_due::pwm_lib;

volatile boolean l=0;
volatile int count=1000;
void sysTick()
{
  count--;
  if(count<-0)
  {
    l = 1;
    count=1000;
  }

  midircv_sysTick();
  win_sysTick();
}

void setup() {

  //************* DCOs ***************************
  dco_init();
  
  //************* UARTS **************************  
  Serial.begin(9600);
  Serial.setTimeout(0);
  midircv_init();
  
  //************* ANALOGS ************************
  ain_init();

  //************* SYSTICK ************************
  Timer2.attachInterrupt(sysTick).setFrequency(1000).start(); // systick 1ms

  //************* SYNTH **************************
  midi_init();
  
  //************** LCD and TOUCH *****************
  pinMode(53, OUTPUT);
  digitalWrite(53, HIGH); // RD en 1
  pinMode(44, OUTPUT);
  pinMode(45, OUTPUT);
  pinMode(46, OUTPUT);
  pinMode(47, OUTPUT);
  pinMode(48, OUTPUT);
  pinMode(49, OUTPUT);
  pinMode(50, OUTPUT);
  pinMode(51, OUTPUT);
  delay(100);
  myGLCD.InitLCD();
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.clrScr();
  myGLCD.fillScr(0x42, 0x42, 0x42);
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
  win_init();

  
  
  pinMode(41, OUTPUT); //debug
  digitalWrite(41, LOW);  


  // init screen
  wm_showBoot();  
}

void loop() {

  win_touchManager();
  win_drawAll();   

  ain_state_machine();
  midircv_stateMachine();
  
  if(l)
  {
    l=0;
    uint16_t* values = ain_getValues();
    /*
    Serial.write("ANALOG 0:");
    Serial.print(values[0],DEC);
    Serial.write("\n");

    Serial.write("ANALOG 1:");
    Serial.print(values[1],DEC);
    Serial.write("\n");

    Serial.write("ANALOG 11:");
    Serial.print(values[11],DEC);
    Serial.write("\n");
    */

    /*
    int potVal = widvc_getMidiValue(&controlAttack);
    Serial.write("ATTACK:");
    Serial.print(potVal,DEC);
    Serial.write("\n");
     */
    
  }
  else
  {
    /*
    pwm_pin34.set_duty_fast(255);
    pwm_pin7.set_duty_fast(255);
    pwm_pin6.set_duty_fast(255);
    pwm_pin8.set_duty_fast(255);
    pwm_pin9.set_duty_fast(255);
    pwm_pin36.set_duty_fast(536);
    pwm_pin38.set_duty_fast(536);
    pwm_pin40.set_duty_fast(536);
    */
  }
  
  
  //delay(1000);
}
