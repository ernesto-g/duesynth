#include <chip.h>
#include "DueTimer.h"
#include "pwm_lib.h"
#include "DcoManager.h"
#include "AnalogIns.h"
#include "MIDIReception.h"
#include "MIDIManager.h"
#include "AdsrManager.h"


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
#include "FrontPanel.h"


void sysTick()
{
  midircv_sysTick();
  win_sysTick();
  fp_sysTick();
}

void setup() {

  delay(3000);

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
  adsr_init();
  midi_init();
  
  //************** LCD and TOUCH *****************
  pinMode(53, OUTPUT);
  digitalWrite(53, HIGH); // RD = 1
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

  //************** DEBUG *****************
  pinMode(21, OUTPUT); //debug
  digitalWrite(21, LOW);  



  //************** Screen Init *****************
  wm_showBoot();    
  fp_init();
}

void loop() {

  win_touchManager();
  win_drawAll();   

  ain_state_machine();
  midircv_stateMachine();
  fp_stateMachine();
    
}
