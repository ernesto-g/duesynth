#include <chip.h>
#include "DueTimer.h"
#include "pwm_lib.h"
#include "DcoManager.h"
#include "AnalogIns.h"
#include "MIDIReception.h"
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

  
  pinMode(2, OUTPUT); 
}

void loop() {

  /*
  unsigned char bufferMidiInternalKeyboard[32];
  digitalWrite(2, HIGH);
  int c = Serial.readBytes(bufferMidiInternalKeyboard,1); // 12uS
  digitalWrite(2, LOW);
  if(c>0)
  {
    Serial.write(bufferMidiInternalKeyboard,c);    
  }*/
  

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

    //digitalWrite(2, LOW);
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
