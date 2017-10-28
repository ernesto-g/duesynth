#include <Arduino.h>

#define MIDI_CMD_NOTE_ON  144
#define MIDI_CMD_NOTE_OFF  128


typedef struct S_MidiInfo {
  byte channel;
  byte cmd;
  byte note;
  byte vel;
}MidiInfo;


void midircv_init(void);
void midircv_stateMachine(void);
void midircv_sysTick(void);


