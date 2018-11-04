

void seq_init(void);
void seq_sysTick(void);
void seq_keyEvent(MidiInfo* pMidiInfo);
void seq_loop(void);
void seq_setState(int s);
void seq_setBpmRate(int rate);
void seq_tapRestEvent(void);


#define SEQ_STATE_OFF     0
#define SEQ_STATE_PLAY    1
#define SEQ_STATE_RECORD  2



