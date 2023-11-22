#ifndef PROJECT_H
#define PROJECT_H


enum states { setLow, setHigh};
enum tStates { cooling, heating, off};
enum servoStates { temp, lower, upper};

extern enum states state;
extern enum tStates tState;
extern enum servoStates servoState;


extern uint8_t farint;
extern uint8_t lowerBound;
extern uint8_t upperBound;
extern volatile uint8_t tempChange;
extern volatile uint8_t settingChange;
extern volatile uint8_t stateChange;
extern volatile uint8_t alarmOff;


extern volatile uint8_t new_state, old_state;
extern volatile uint8_t tempChangeBool;
extern volatile uint8_t a, b;
extern volatile uint8_t x;
extern volatile uint8_t numISRtoggles;


char checkInputD(char bit);

#endif