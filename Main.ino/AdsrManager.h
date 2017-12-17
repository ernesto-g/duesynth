

void adsr_stateMachineTick(void);
void adsr_init(void);

void adsr_gateOnEvent(void);
void adsr_gateOffEvent(void);
void adsr_triggerEvent(int vel);


void adsr_setAnalogAttackRate(int i, int value);
void adsr_setAnalogDecayRate(int i, int value);
void adsr_setAnalogReleaseRate(int i, int value);
void adsr_setAnalogSustainValue(int i, int value);

