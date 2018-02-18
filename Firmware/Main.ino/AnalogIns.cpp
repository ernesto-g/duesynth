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

#include <Arduino.h>
#include <HardwareSerial.h>
#include "AnalogIns.h"

#define REAL_CHANNELS 12
#define CHANNELS    16

#define AN_MUX_PIN_A  11
#define AN_MUX_PIN_B  12
#define AN_MUX_PIN_C  13

static uint16_t inputsValue[CHANNELS];
static uint8_t currentChn;
static uint8_t state;
static uint8_t isReady;
static uint16_t timeoutWaitChannelMux;

static void disablePrevChannel(int channel);
static void enableChannel(int channel);
static void setMuxAddress(int addr);


void ain_init(void)
{
  pinMode(AN_MUX_PIN_A, OUTPUT);
  pinMode(AN_MUX_PIN_B, OUTPUT);
  pinMode(AN_MUX_PIN_C, OUTPUT);
    
  currentChn=0;
  analogRead(0); // read for initialization
  state = ANALOG_STATE_IDLE;
  isReady=0;
}

void ain_state_machine(void)
{
    switch(state)
    {
      case ANALOG_STATE_IDLE:
      {
        // disable prev channel
        disablePrevChannel(currentChn);
        
        state = ANALOG_STATE_WAIT_CHN_MUX;
        timeoutWaitChannelMux = 1;
        break;  
      }
      case ANALOG_STATE_WAIT_CHN_MUX:
      {
        timeoutWaitChannelMux--;
        if(timeoutWaitChannelMux==0)
        {
          state = ANALOG_STATE_WAIT_CHN_MUX2; //ANALOG_STATE_START;
          // Enable current channel
          enableChannel(currentChn);
          timeoutWaitChannelMux = 1;
        }
        break;
      }
      case ANALOG_STATE_WAIT_CHN_MUX2:
      {
        timeoutWaitChannelMux--;
        if(timeoutWaitChannelMux==0)
        {
          state = ANALOG_STATE_START;          
        }
        break;
      }
      case ANALOG_STATE_START:
      {
        // Start conversion
        adc_start( ADC );
        state = ANALOG_STATE_WAIT;
        break;
      }
      case ANALOG_STATE_WAIT:
      {
        if(((adc_get_status(ADC) & ADC_ISR_DRDY) == ADC_ISR_DRDY) )
        {
          inputsValue[currentChn] = adc_get_latest_value(ADC); //(high << 8) | low;
          state = ANALOG_STATE_FINISH;
        }
        break;
      }
      case ANALOG_STATE_FINISH:
      {
        //Serial.print("Valor entrada ");
        //Serial.print(currentChn,DEC);
        //Serial.print(": ");
        //Serial.print(inputsValue[currentChn],DEC);
        //Serial.print("\r\n");
                
        currentChn++;
        if(currentChn>=CHANNELS){
          currentChn=0;
          isReady=1;
        }
        state = ANALOG_STATE_IDLE;  
        break;
      }
    }
}

uint16_t* ain_getValues(void)
{
  return inputsValue;
}

uint8_t ain_isReady(void)
{
  return isReady;
}



static void disablePrevChannel(int channel)
{
  if(channel<REAL_CHANNELS)
  {
      int prevChn = ((int)channel)-1;
      if(prevChn<0)
        prevChn=(REAL_CHANNELS-1);

      //Serial.print("Deshabilito chn:");
      //Serial.print(g_APinDescription[A0 + prevChn].ulADCChannelNumber,DEC);
      adc_disable_channel( ADC, (adc_channel_num_t) g_APinDescription[A0 + prevChn].ulADCChannelNumber );
  }
}

static void enableChannel(int channel)
{
  if(channel>=(REAL_CHANNELS-1))
  {
    switch(channel)
    {
      case 11:
        setMuxAddress(0);
        break;
      case 12:
        setMuxAddress(1);
        break;
      case 13:
        setMuxAddress(2);
        break;
      case 14:
        setMuxAddress(3);
        break;
      case 15:
        setMuxAddress(4);
        break;        
    }
    channel = REAL_CHANNELS-1; // set channel 11 (through analog MUX)
  }
  
  //Serial.print("Habilito chn:");
  //Serial.print(g_APinDescription[A0 + currentChn].ulADCChannelNumber,DEC);
  adc_enable_channel( ADC,(adc_channel_num_t) g_APinDescription[A0 + channel].ulADCChannelNumber );
}



 
static void setMuxAddress(int addr)
{
  switch(addr)
  {
    case 0:
      digitalWrite(AN_MUX_PIN_A, LOW); // A=0
      digitalWrite(AN_MUX_PIN_B, LOW); // B=0
      digitalWrite(AN_MUX_PIN_C, LOW); // C=0       
      break;
    case 1:
      digitalWrite(AN_MUX_PIN_A, HIGH); // A=1
      digitalWrite(AN_MUX_PIN_B, LOW); // B=0
      digitalWrite(AN_MUX_PIN_C, LOW); // C=0     
      break;
    case 2:
      digitalWrite(AN_MUX_PIN_A, LOW); // A=0
      digitalWrite(AN_MUX_PIN_B, HIGH); // B=1
      digitalWrite(AN_MUX_PIN_C, LOW); // C=0     
      break;
    case 3:
      digitalWrite(AN_MUX_PIN_A, HIGH); // A=1
      digitalWrite(AN_MUX_PIN_B, HIGH); // B=1
      digitalWrite(AN_MUX_PIN_C, LOW); // C=0     
      break;
    case 4:
      digitalWrite(AN_MUX_PIN_A, LOW); // A=0
      digitalWrite(AN_MUX_PIN_B, LOW); // B=0
      digitalWrite(AN_MUX_PIN_C, HIGH); // C=1     
      break;
    case 5:
      digitalWrite(AN_MUX_PIN_A, HIGH); // A=1
      digitalWrite(AN_MUX_PIN_B, LOW); // B=0
      digitalWrite(AN_MUX_PIN_C, HIGH); // C=1     
      break;
    case 6:
      digitalWrite(AN_MUX_PIN_A, LOW); // A=0
      digitalWrite(AN_MUX_PIN_B, HIGH); // B=1
      digitalWrite(AN_MUX_PIN_C, HIGH); // C=1     
      break;
    case 7:
      digitalWrite(AN_MUX_PIN_A, HIGH); // A=1
      digitalWrite(AN_MUX_PIN_B, HIGH); // B=1
      digitalWrite(AN_MUX_PIN_C, HIGH); // C=1     
      break;
  }
}

