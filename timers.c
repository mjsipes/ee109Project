
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "project.h"
#include "timers.h"

//-------------------------BUZZER OUTPUT-------------------------------BUZZER OUTPUT--------------------
//-------------------------BUZZER OUTPUT-------------------------------BUZZER OUTPUT--------------------
void play_note(uint16_t freq)
{
	numISRtoggles = freq;
	timer0_init();
	OCR0A = 16000000 / (freq*2);
	TCCR0B |= 0b00000100;
}
void timer0_init()
{
    TCCR0A |= (1 << WGM01);
    TIMSK0 |= (1 << OCIE0A);
}
ISR(TIMER0_COMPA_vect)
{
	if (numISRtoggles == 0) TCCR0B &= ~(0b00000111);
	PORTB ^= (1 << PB5);
	numISRtoggles--;
}
//-------------------------BUZZER OUTPUT-------------------------------BUZZER OUTPUT--------------------
//-------------------------BUZZER OUTPUT-------------------------------BUZZER OUTPUT--------------------

//-------------------SERVOMOTOR 4 SECOND WAIT-----------------------SERVOMOTOR 4 SECOND WAIT--------------------
//-------------------SERVOMOTOR 4 SECOND WAIT-----------------------SERVOMOTOR 4 SECOND WAIT--------------------
void timer1_init()
{
	TCCR1B |= 0b101;
    TCCR1B |= (1 << WGM12);
	OCR1A = 62500;
    TIMSK1 |= (1 << OCIE1A);
}
ISR(TIMER1_COMPA_vect)
{
 	servoState = temp;
	TCCR1B &= ~(0b00000111);
}
//-------------------SERVOMOTOR 4 SECOND WAIT-----------------------SERVOMOTOR 4 SECOND WAIT--------------------
//-------------------SERVOMOTOR 4 SECOND WAIT-----------------------SERVOMOTOR 4 SECOND WAIT--------------------

//-------------------SERVOMOTOR PWM-----------------------SERVOMOTOR PWM--------------------
//-------------------SERVOMOTOR PWM-----------------------SERVOMOTOR PWM--------------------
void timer2_init(void)
{
	TCCR2A |= (0b11 << WGM20);  
    TCCR2A |= (0b10 << COM2A0); 
    TCCR2B |= (0b111 << CS20);
	OCR2A = 23;  
}
//-------------------SERVOMOTOR PWM-----------------------SERVOMOTOR PWM--------------------
//-------------------SERVOMOTOR PWM-----------------------SERVOMOTOR PWM--------------------






