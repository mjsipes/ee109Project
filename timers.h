
void play_note(uint16_t freq);
void timer0_init(void);
ISR(TIMER0_COMPA_vect);
void timer1_init(void);
ISR(TIMER1_COMPA_vect);
void timer2_init(void);