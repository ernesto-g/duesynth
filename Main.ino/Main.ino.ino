#include <chip.h>
#include "DueTimer.h"
#include "pwm_lib.h"
#include "DcoManager.h"
using namespace arduino_due::pwm_lib;

volatile boolean l=0;
void tmr3Handler()
{
  l = !l;
}



void setup() {

  dco_init();
  
//************** TIMERS *************************
  //Timer3.attachInterrupt(tmr3Handler).setFrequency(1).start();

  //************* UARTS **************************  
  Serial.begin(9600);
  Serial.setTimeout(10);
  Serial1.begin(31250);
  Serial1.setTimeout(0); // minimo 5ms


  pinMode(2, OUTPUT); 

}

void loop() {

  unsigned char bufferMidiInternalKeyboard[32];
  int c = Serial.readBytes(bufferMidiInternalKeyboard,32); // se bloquea el tiempo del timeout seteado
  if(c>0)
  {
    Serial.write(bufferMidiInternalKeyboard,c);    
  }

    int v = analogRead(0); // tarda 6uS

  if(l)
  {
    //Serial.write("hola\n");
    /*
    pwm_pin34.set_duty_fast(143); // tarda 0.7uS  572 ok
    pwm_pin7.set_duty_fast(128);
    pwm_pin6.set_duty_fast(128);
    pwm_pin8.set_duty_fast(128);
    pwm_pin9.set_duty_fast(128);
    pwm_pin36.set_duty_fast(340);
    pwm_pin38.set_duty_fast(340);
    pwm_pin40.set_duty_fast(340);
    */
    digitalWrite(2, LOW);
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
