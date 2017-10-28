
#include "MIDIReception.h"
#include "MIDIManager.h"
#include "DcoManager.h"

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
                dco_setMIDInote(pMidiInfo->note);
            }
        }
        else if(pMidiInfo->cmd==MIDI_CMD_NOTE_OFF)
        {
            // NOTE OFF
            if(pMidiInfo->note>=21 && pMidiInfo->note<=96)
            { 
              
            }              
        }
    }
}

