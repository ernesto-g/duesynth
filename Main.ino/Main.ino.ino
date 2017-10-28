#include <chip.h>
#include "DueTimer.h"
#include "pwm_lib.h"
#include "DcoManager.h"
#include "AnalogIns.h"
using namespace arduino_due::pwm_lib;

volatile boolean l=0;
void tmr2Handler()
{
  l = !l;
}



void setup() {

  //************* DCOs ***************************
  dco_init();
  
  //************* UARTS **************************  
  Serial.begin(9600);
  Serial.setTimeout(10);
  Serial1.begin(31250);
  Serial1.setTimeout(0); // minimo 5ms

  //************* ANALOGS ************************
  ain_init();


  // debug
  Timer2.attachInterrupt(tmr2Handler).setFrequency(1).start(); // freq update: 72Khz
  
  pinMode(2, OUTPUT); 
}

void loop() {

  unsigned char bufferMidiInternalKeyboard[32];
  int c = Serial.readBytes(bufferMidiInternalKeyboard,32); // se bloquea el tiempo del timeout seteado
  if(c>0)
  {
    Serial.write(bufferMidiInternalKeyboard,c);    
  }

  ain_state_machine();

  if(l)
  {
    l=0;
    uint16_t* values = ain_getValues();
    Serial.write("ANALOG 0:");
    Serial.print(values[0],DEC);
    Serial.write("\n");

    Serial.write("ANALOG 1:");
    Serial.print(values[1],DEC);
    Serial.write("\n");

    Serial.write("ANALOG 11:");
    Serial.print(values[11],DEC);
    Serial.write("\n");


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
