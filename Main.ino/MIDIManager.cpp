#include "MIDIReception.h"
#include "MIDIManager.h"
#include "DcoManager.h"

// Types
typedef struct S_KeyPressedInfo {
  byte note;
  byte flagFree; 
}KeyPressedInfo;

// Defines
#define KEYS_PRESSED_LEN    11

#define NOTE_PRIORITY_LAST  0
#define NOTE_PRIORITY_HIGH  1
#define NOTE_PRIORITY_LOW   2

#define ENVELOPE_LEGATO_MODE_ON   0
#define ENVELOPE_LEGATO_MODE_OFF  1

#define LFO_SYNC_MODE_ON    0
#define LFO_SYNC_MODE_OFF   1


// Private functions
static byte saveKey(byte note);
static byte getIndexOfPressedKey(byte note);
static byte deleteKey(byte note);
static byte thereAreNoKeysPressed(void);
static byte getTheLowestKeyPressed(void);
static byte getTheHighestKeyPressed(void);

// Private vaiables
static KeyPressedInfo keysPressed[KEYS_PRESSED_LEN];
static byte notePriority;
static byte envelopeLegatoMode;
static byte lfoSyncMode;


//Functions
void midi_init(void)
{
    int i;
    notePriority = NOTE_PRIORITY_LAST;
    envelopeLegatoMode = ENVELOPE_LEGATO_MODE_OFF;
    lfoSyncMode=LFO_SYNC_MODE_ON;

    for(i=0; i<KEYS_PRESSED_LEN; i++)
      keysPressed[i].flagFree=1;
}


void midi_analizeMidiInfo(MidiInfo * pMidiInfo)
{
    //if(pMidiInfo->channel==MIDI_CURRENT_CHANNEL)
    {
        if(pMidiInfo->cmd==MIDI_CMD_NOTE_ON)
        {
            Serial.write("llego comando:");
            Serial.print(pMidiInfo->cmd,DEC);
            Serial.write("llego NOTA:");
            Serial.print(pMidiInfo->note,DEC);
            Serial.write("llego vel:");
            Serial.print(pMidiInfo->vel,DEC);
            Serial.write("\n");
            
            // NOTE ON 
            if(pMidiInfo->note>=21 && pMidiInfo->note<=96)
            { 
                // priority
                if(notePriority==NOTE_PRIORITY_LOW)
                {
                    if(pMidiInfo->note > getTheLowestKeyPressed())
                    {
                      if(envelopeLegatoMode==ENVELOPE_LEGATO_MODE_OFF) //OFF: the envelopes are triggered each time a key is pressed.
                        dco_setTrigger(pMidiInfo->vel); 
                        
                      saveKey(pMidiInfo->note);
                      return; // ignore key                         
                    }
                }
                else if(notePriority==NOTE_PRIORITY_HIGH)
                {
                    if(pMidiInfo->note < getTheHighestKeyPressed())
                    {
                      if(envelopeLegatoMode==ENVELOPE_LEGATO_MODE_OFF) //OFF: the envelopes are triggered each time a key is pressed.
                        dco_setTrigger(pMidiInfo->vel); 
                        
                      saveKey(pMidiInfo->note);
                      return; // ignore key                         
                    }
                }
                //__________

                // trigger and gate
                dco_setGate(1); // gate=1
                if(envelopeLegatoMode==ENVELOPE_LEGATO_MODE_OFF)  // OFF: the envelopes are triggered each time a key is pressed.
                  dco_setTrigger(pMidiInfo->vel);
                else  // ON: if a new note is played while the preceding one is still active, the envelopes will not be triggered again.
                {
                    if(thereAreNoKeysPressed())
                      dco_setTrigger(pMidiInfo->vel);
                }
                //_________________

                if(lfoSyncMode==ENVELOPE_LEGATO_MODE_ON)
                  dco_lfoReset();
                                
                saveKey(pMidiInfo->note);

                dco_setMIDInote(pMidiInfo->note);
            }
        }
        else if(pMidiInfo->cmd==MIDI_CMD_NOTE_OFF)
        {
            // NOTE OFF
            if(pMidiInfo->note>=21 && pMidiInfo->note<=96)
            { 
                if(deleteKey(pMidiInfo->note))
                {
                  if(thereAreNoKeysPressed())
                  {
                      dco_setGate(0); // gate=0
                  }
                  else
                  {
                    if(notePriority==NOTE_PRIORITY_LOW)
                    {
                      // a key was released. keep playing previous lower key
                      byte previousNote = getTheLowestKeyPressed();
                      if(previousNote!=0xFF)
                        dco_setMIDInote(previousNote);
                    }
                    else if(notePriority==NOTE_PRIORITY_HIGH)
                    {
                      // a key was released. keep playing previous highest key
                      byte previousNote = getTheHighestKeyPressed();
                      if(previousNote!=0xFF)
                        dco_setMIDInote(previousNote);
                    }
                  }
                }
            }              
        }
    }
}


/// Keys management
static byte saveKey(byte note)
{
  byte i;
  for(i=0; i<KEYS_PRESSED_LEN; i++)
  {
    if(keysPressed[i].flagFree==1)
    {
        keysPressed[i].flagFree=0;
        keysPressed[i].note = note;
        return 0;
    }
  }
  return 1; // no more space
}
static byte getIndexOfPressedKey(byte note)
{
  byte i;
  for(i=0; i<KEYS_PRESSED_LEN; i++)
  {
    if(keysPressed[i].flagFree==0)
    {
        if(keysPressed[i].note == note)
          return i;
    }
  }
  return 0xFF; 
}
static byte deleteKey(byte note)
{
    byte index = getIndexOfPressedKey(note);
    if(index<KEYS_PRESSED_LEN)
    {
      keysPressed[index].flagFree=1;
      return 1;
    }
    return 0;
}
static byte thereAreNoKeysPressed(void)
{
  byte i;
  for(i=0; i<KEYS_PRESSED_LEN; i++)
  {
    if(keysPressed[i].flagFree==0)
    {
      return 0;
    }
  }
  return 1; 
}
static byte getTheLowestKeyPressed(void)
{
    byte i;
    byte mi = 0xFF;
    for(i=0; i<KEYS_PRESSED_LEN; i++)
    {
      if(keysPressed[i].flagFree==0)
      {
          if(keysPressed[i].note<mi)
            mi = keysPressed[i].note;
      }
    }
    return mi;
}
static byte getTheHighestKeyPressed(void)
{
    byte i;
    byte max = 0x00;
    for(i=0; i<KEYS_PRESSED_LEN; i++)
    {
      if(keysPressed[i].flagFree==0)
      {
          if(keysPressed[i].note>max)
            max = keysPressed[i].note;
      }
    }
    return max;
}
//____________________________________________


