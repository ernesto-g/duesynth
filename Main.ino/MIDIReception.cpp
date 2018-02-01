#include <Arduino.h>
#include <HardwareSerial.h>
#include "MIDIReception.h"
#include "MIDIManager.h"
#include "Outs.h"

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
}
void midircv_subOctaveOffset(void)
{
  if(octaveOffsetLocalKeyboard>(-2))
    octaveOffsetLocalKeyboard--;

  updateOctaveLeds();
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
  }
  
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

