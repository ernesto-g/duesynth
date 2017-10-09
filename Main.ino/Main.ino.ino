#include <chip.h>
#include "DueTimer.h"
#include "pwm_lib.h"
using namespace arduino_due::pwm_lib;

volatile boolean l=0;
void tmr3Handler()
{
  l = !l;
}

pwm<pwm_pin::PWML0_PC2> pwm_pin34;
pwm<pwm_pin::PWML1_PC4> pwm_pin36;
pwm<pwm_pin::PWML2_PC6> pwm_pin38;
pwm<pwm_pin::PWML3_PC8> pwm_pin40;
pwm<pwm_pin::PWML4_PC21> pwm_pin9;
pwm<pwm_pin::PWML5_PC22> pwm_pin8;
pwm<pwm_pin::PWML6_PC23> pwm_pin7;
pwm<pwm_pin::PWML7_PC24> pwm_pin6;


void setup() {
//************** TIMERS *************************
  Timer3.attachInterrupt(tmr3Handler).setFrequency(1).start();

  //************* UARTS **************************  
  Serial.begin(9600);
  Serial.setTimeout(10);
  Serial1.begin(31250);
  Serial1.setTimeout(10);

  //*********** PWMs ****************************
  pwm_pin34.start(680,340);
  pwm_pin7.start(680,340);
  pwm_pin6.start(680,340);
  pwm_pin8.start(680,340);
  pwm_pin9.start(680,340);
  pwm_pin36.start(680,340);
  pwm_pin38.start(680,340);
  pwm_pin40.start(680,340);

}

void loop() {

  unsigned char bufferMidiInternalKeyboard[32];
  int c = Serial.readBytes(bufferMidiInternalKeyboard,32);
  if(c>0)
  {
    Serial.write(bufferMidiInternalKeyboard,c);    
  }

  if(l)
  {
    Serial.write("hola\n");
    pwm_pin34.set_duty(128);
    pwm_pin7.set_duty(128);
    pwm_pin6.set_duty(128);
    pwm_pin8.set_duty(128);
    pwm_pin9.set_duty(128);
    pwm_pin36.set_duty(340);
    pwm_pin38.set_duty(340);
    pwm_pin40.set_duty(340);
  }
  else
  {
    pwm_pin34.set_duty(255);
    pwm_pin7.set_duty(255);
    pwm_pin6.set_duty(255);
    pwm_pin8.set_duty(255);
    pwm_pin9.set_duty(255);
    pwm_pin36.set_duty(639);
    pwm_pin38.set_duty(639);
    pwm_pin40.set_duty(639);
  }
  
  
  //delay(1000);
}
