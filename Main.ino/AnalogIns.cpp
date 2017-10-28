/**
 *   Arduino Due Synth
    Copyright (C) <2017>  Ernesto Gigliotti <ernestogigliotti@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <Arduino.h>
#include <HardwareSerial.h>
#include "AnalogIns.h"

#define CHANNELS    12

static uint16_t inputsValue[CHANNELS];
static uint8_t currentChn;
static uint8_t state;
static uint8_t isReady;
static uint16_t timeoutWaitChannelMux;

void ain_init(void)
{
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
        // set channel
        /*
        ADMUX = ADMUX & 0xF0;
        ADMUX = ADMUX | (currentChn & 0x0F);
        */
        int prevChn = ((int)currentChn)-1;
        if(prevChn<0)
          prevChn=(CHANNELS-1);

        //Serial.print("Deshabilito chn:");
        //Serial.print(g_APinDescription[A0 + prevChn].ulADCChannelNumber,DEC);
        adc_disable_channel( ADC, (adc_channel_num_t) g_APinDescription[A0 + prevChn].ulADCChannelNumber );
        
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
          //Serial.print("Habilito chn:");
          //Serial.print(g_APinDescription[A0 + currentChn].ulADCChannelNumber,DEC);
          adc_enable_channel( ADC,(adc_channel_num_t) g_APinDescription[A0 + currentChn].ulADCChannelNumber );
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
        //ADCSRA|=0x40;
        adc_start( ADC );
        state = ANALOG_STATE_WAIT;
        break;
      }
      case ANALOG_STATE_WAIT:
      {
        //while ((adc_get_status(ADC) & ADC_ISR_DRDY) != ADC_ISR_DRDY);
        
        if(((adc_get_status(ADC) & ADC_ISR_DRDY) == ADC_ISR_DRDY) )
        {
          /*
          uint8_t low, high;
          low  = ADCL;
          high = ADCH;
          */
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


