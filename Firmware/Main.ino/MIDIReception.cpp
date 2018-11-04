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
#include "MIDIReception.h"
#include "MIDIManager.h"
#include "Outs.h"
#include "SequencerManager.h"


// USB MIDI lib
#include "MIDIUSB/MIDIUSB.h"
#include "MIDIUSB/MIDIUSB.cpp"
//_____________


#define MIDI_BUFFER_LEN   32
#define FROM_INTERNAL_KEYBOARD  0
#define FROM_EXTERNAL_KEYBOARD  1

// Private functions
static void processMidiPacket(unsigned char* pData, int len, int fromKeyboard,MidiInfo* pMidiInfo);
static void updateOctaveLeds(void);

// Private variables
static unsigned char bufferMidiInternalKeyboard[MIDI_BUFFER_LEN];
static unsigned char bufferMidiExternalKeyboard[MIDI_BUFFER_LEN];
static unsigned int indexBufferInternal;
static unsigned int indexBufferExternal;
static int octaveOffsetLocalKeyboard;

static unsigned char midiOutChannel;

void midircv_sysTick(void)
{
  
}

void midircv_init(void)
{
  // Internal keyboard
  Serial1.begin(31250);
  Serial1.setTimeout(0);
  indexBufferInternal=0;

  // External keyboard
  Serial2.begin(31250);
  Serial2.setTimeout(0);
  indexBufferExternal=0;

  octaveOffsetLocalKeyboard=0;
  updateOctaveLeds();

  midiOutChannel = 0; // MIDI OUT CHANNEL FOR INTERNAL KEYBOARD
}

void midircv_stateMachine(void)
{
  unsigned char data;
  int c;

  //digitalWrite(2, HIGH);
  //digitalWrite(2, LOW);
  
  c = Serial1.readBytes(&data,1); // 12uS
  if(c>0)
  {
    bufferMidiInternalKeyboard[indexBufferInternal] = data;
    indexBufferInternal++;
    if(indexBufferInternal==3)
    {
      //Serial.write("PACKET!");
      MidiInfo midiInfo;
      processMidiPacket(bufferMidiInternalKeyboard,indexBufferInternal,FROM_INTERNAL_KEYBOARD,&midiInfo);      
      midi_analizeMidiInfo(&midiInfo);
      indexBufferInternal=0;
    }
  }

  c = Serial2.readBytes(&data,1); // 12uS
  if(c>0)
  {
    bufferMidiExternalKeyboard[indexBufferExternal] = data;
    indexBufferExternal++;
    if(indexBufferExternal==3)
    {
      MidiInfo midiInfo;
      processMidiPacket(bufferMidiExternalKeyboard,indexBufferExternal,FROM_EXTERNAL_KEYBOARD,&midiInfo);
      midi_analizeMidiInfo(&midiInfo);      
      indexBufferExternal=0;
    }
  }
}

void midircv_resetOctaveOffset(void)
{
  octaveOffsetLocalKeyboard=0;
  updateOctaveLeds();  
}
void midircv_addOctaveOffset(void)
{
  if(octaveOffsetLocalKeyboard<2)
    octaveOffsetLocalKeyboard++;

  updateOctaveLeds();
  midi_clearAllKeysPressed();
}
void midircv_subOctaveOffset(void)
{
  if(octaveOffsetLocalKeyboard>(-2))
    octaveOffsetLocalKeyboard--;

  updateOctaveLeds();
  midi_clearAllKeysPressed();
}

static void processMidiPacket(unsigned char* pData, int len, int fromKeyboard,MidiInfo* pMidiInfo)
{
  // 1st byte
  pMidiInfo->channel = pData[0] & B00001111;
  pMidiInfo->cmd = pData[0] & B11110000;

  // 2nd byte
  pMidiInfo->note = pData[1];
  
  // 3rd byte
  pMidiInfo->vel = pData[2];

  // fix internal keyboard 
  if(fromKeyboard==FROM_INTERNAL_KEYBOARD) // internal keyboard notes 36 to 60
  {
    if(pMidiInfo->note == 72) // keyboard fix (this keyboard is broken and last key is 72 instead 60). comment for normal key
      pMidiInfo->note = 60;

    // octave offset
    pMidiInfo->note = pMidiInfo->note + (octaveOffsetLocalKeyboard*12);  
    pMidiInfo->note+= 12; // center octave for internal keyboard  

    // send to MIDI OUT
    unsigned char b = pMidiInfo->cmd | midiOutChannel;
    Serial2.write(&b,1); 
    Serial2.write(&(pMidiInfo->note),1);
    Serial2.write(&pData[2],1);
    //_________________
  }
  else
  {
    // send to MIDI OUT
    Serial2.write(pData,len);
    //_________________        
  }

  // Send by USB MIDI
  midiEventPacket_t packet = {(pMidiInfo->cmd>>4), pMidiInfo->cmd | pMidiInfo->channel, pMidiInfo->note, pMidiInfo->vel};
  MidiUSB.sendMIDI(packet);
  MidiUSB.flush();
  //_________________

  // Send to sequencer manager
  seq_keyEvent(pMidiInfo);
  //__________________________      
}

static void updateOctaveLeds(void)
{
  outs_set(OUT_LED_OCTAVE_N2,0);
  outs_set(OUT_LED_OCTAVE_N1,0);
  outs_set(OUT_LED_OCTAVE_0,0);
  outs_set(OUT_LED_OCTAVE_P1,0);
  outs_set(OUT_LED_OCTAVE_P2,0);
  
  switch(octaveOffsetLocalKeyboard)
  {
    case -2: outs_set(OUT_LED_OCTAVE_N2,1);break;
    case -1: outs_set(OUT_LED_OCTAVE_N1,1);break;
    case 0: outs_set(OUT_LED_OCTAVE_0,1);break;
    case 1: outs_set(OUT_LED_OCTAVE_P1,1);break;
    case 2: outs_set(OUT_LED_OCTAVE_P2,1);break;    
  }
}

