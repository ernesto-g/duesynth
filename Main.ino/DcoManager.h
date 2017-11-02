
#define LFO_WAVE_TYPE_SINE        0
#define LFO_WAVE_TYPE_TRIANGLE    1
#define LFO_WAVE_TYPE_EXP         2
#define LFO_WAVE_TYPE_SQUARE      3

void dco_init(void);

void dco_setMIDInote(int note);


void dco_setPwmForSquare(unsigned int analogValue);
void dco_setPhaseForUltrasaw(unsigned int analogValue);


void dco_setGate(byte val);
void dco_setTrigger(byte velocity);

void dco_lfoReset(void);
void dco_lfoFreq(byte value);
void dco_lfoSetWaveType(byte type);





