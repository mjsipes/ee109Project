/********************************************
 *
 *  Name: Michael Sipes
 *  Email: msipes@usc.edu
 *  Section: WED 12:30
 *  Assignment: final project
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "project.h"
#include "lcd.h"
#include "ds18b20.h"
#include "encoder.h"
#include "timers.h"



// enum states { setLow, setHigh};
// enum tStates { cooling, heating, off};
// enum servoStates { temp, lower, upper};

enum states state;
enum tStates tState;
enum servoStates servoState;

uint8_t farint; 
uint8_t lowerBound;
uint8_t upperBound;
volatile uint8_t tempChange;
volatile uint8_t settingChange;
volatile uint8_t stateChange;
volatile uint8_t alarmOff;


volatile uint8_t new_state, old_state;
volatile uint8_t tempChangeBool;
volatile uint8_t a, b;
volatile uint8_t x;
volatile uint8_t numISRtoggles;

uint16_t frequency[8] = { 262, 294, 330, 349, 392, 440, 494, 523 };







int main(void) {
	state = setLow;
	tState = off;
	servoState = temp;
	farint = 0; 
	tempChange = 1;
	settingChange = 0;
	stateChange = 0;
	alarmOff = 0;
	tempChangeBool = 1;
	numISRtoggles = 0;


    // Initialize LCD screem
	lcd_init();
	//initialize timer2 for the servomotor
	timer2_init();
	// set output pin for speaker and servor motor
	DDRB |= ((1 << 5) | (1 << 3));
	//set output pins for LED ports
	DDRC |= ((1 << 3) | (1 << 4) | (1 << 5));
	PORTC |= ((1 << 3) | (1 << 4) | (1 << 5));
	//set pull up resistors for encoder inputs
	PORTC |= ((1 << PC1) | (1 << PC2));
	//set pull up resistors for buttons
	PORTD |= ((1 << 2) | (1 << 3));
	//enable pinchange interrupts for rotary encoder
	PCICR |= (1 << PCIE1);
	PCMSK1 |= ((1 << PCINT2)|(1 << PCINT1));
	//enable global interrupts
	sei();

	//read in temperature settings from the eeprom
	lowerBound = eeprom_read_byte((void *) 1);
	upperBound= eeprom_read_byte((void *) 2);
	if (lowerBound < 50 || lowerBound > 90) lowerBound = 50;
	if (upperBound > 90 || upperBound < 50) upperBound = 90;
	if (lowerBound > upperBound) lowerBound = upperBound;

    // Write a spash screen to the LCD
	lcd_writecommand(1);
    lcd_moveto(0,0);
    lcd_stringout("EE 109 Final PA!");
    lcd_moveto(1,0);
    lcd_stringout("Michael Sipes");
    _delay_ms(2000);
	lcd_writecommand(1);

	//display initial temperature settings on LCD screen
	char buf1[18];
	lcd_moveto(0,0);
	snprintf(buf1, 18, "Temp= ");
	lcd_stringout(buf1);
	lcd_moveto(1,0);
	snprintf(buf1, 18, "Low->%02d High= %02d", lowerBound, upperBound);
	lcd_stringout(buf1);

    // Read the A and B inputs to determine the intial state of the rotary encoder
	x = PINC;
	a = (x & (1 << 1)) != 0;  
	b = (x & (1 << 2)) != 0;  
    if (!b && !a)
	old_state = 0;
    else if (!b && a)
	old_state = 1;
    else if (b && !a)
	old_state = 2;
    else
	old_state = 3;
    new_state = old_state;

	// Initialize the DS18B20 and start the first temperature conversion
	unsigned char t[2];
    if (ds_init() == 0) {
		lcd_writecommand(1);
		lcd_writecommand(2);
    	lcd_stringout("Sensor not responding");
    	_delay_ms(2000);
    }
    ds_convert();


	while(1){
		//-------------------------BUTTON INPUT-------------------------------BUTTON INPUT--------------------
		//-------------------------BUTTON INPUT-------------------------------BUTTON INPUT--------------------
		//checks to see if a button was pressed. if so, it changes state so that the rotary encoder affects 
		//the upper or lower temperature setting and it also triggers the servo motor to attend to that 
		//setting for 4 seconds
		if (checkInputD(2)){
			state = setLow;
			servoState = lower;
			timer1_init();
			lcd_moveto(1,3);
			lcd_stringout("->");
			lcd_moveto(1,12);
			lcd_stringout("= ");
		}
		if (checkInputD(3)){
			state = setHigh;
			servoState = upper;
			timer1_init();
			lcd_moveto(1,3);
			lcd_stringout("= ");
			lcd_moveto(1,12);
			lcd_stringout("->");
		}
		//-------------------------BUTTON INPUT-------------------------------BUTTON INPUT--------------------
		//-------------------------BUTTON INPUT-------------------------------BUTTON INPUT--------------------
		//-------------------------TEMP INPUT-------------------------------TEMP INPUT--------------------
		//-------------------------TEMP INPUT-------------------------------TEMP INPUT--------------------
		//reads the new temperature, updates LCD screen, and starts next conversion
		if (ds_temp(t)) {    // True if conversion complete
			uint8_t byte0 = t[0];
			uint8_t byte1 = t[1];
			uint16_t byte2 = (byte1 << 8) | byte0; // = Celsius * 16
			uint16_t far16 = (byte2 * 9 + 32 * 16 * 5)/5; //= farenheit *16
			farint = far16 / 16; //  = farenheit
			uint16_t fardec = (far16 % 16) * 10 / 16; // = farenheit decimal portion
			lcd_moveto(0,6);
			char buf1[16];
			snprintf(buf1, 16, "%02d.%01d", farint, fardec);
			lcd_stringout(buf1);
            ds_convert();   // Start next conversion
        }
		//-------------------------TEMP INPUT-------------------------------TEMP INPUT--------------------
		//-------------------------TEMP INPUT-------------------------------TEMP INPUT--------------------
		//-------------------------LCD OUTPUT-------------------------------LCD OUTPUT--------------------
		//-------------------------LCD OUTPUT-------------------------------LCD OUTPUT--------------------
		//updates the LCD screen if one of the temperature settings was changed
        if (tempChangeBool) {
			tempChangeBool = 0;
			char buf[10];
			snprintf(buf, 4, "%02d", lowerBound);
			lcd_moveto(1,5);
			lcd_stringout(buf);
			snprintf(buf1, 4, "%02d",upperBound);
			lcd_moveto(1,14);
			lcd_stringout(buf1);
        }
		//-------------------------LCD OUTPUT-------------------------------LCD OUTPUT--------------------
		//-------------------------LCD OUTPUT-------------------------------LCD OUTPUT--------------------
		//-------------------------BUZZER OUTPUT-------------------------------BUZZER OUTPUT--------------------
		//-------------------------BUZZER OUTPUT-------------------------------BUZZER OUTPUT--------------------
		//handles logic to set off buzzer alarm if temperature goes out of range and to rearm buzzer alarm
		//when temperature comes back into range
		if (!alarmOff){
			if (farint < (lowerBound -3) || (farint > upperBound + 3)){
				alarmOff = 1;
				play_note(frequency[3]);
			}
		}
		else if (alarmOff){
			if (farint > (lowerBound -3) && (farint < upperBound + 3)){
				alarmOff = 0;
			}
		}
		//-------------------------BUZZER OUTPUT-------------------------------BUZZER OUTPUT--------------------
		//-------------------------BUZZER OUTPUT-------------------------------BUZZER OUTPUT--------------------
		//-------------------------LED OUTPUT-------------------------------LED OUTPUT--------------------
		//-------------------------LED OUTPUT-------------------------------LED OUTPUT--------------------
		//handles logic to display red, green or blue depending on the temperature relative to temperature settings
		if (farint < lowerBound){
			if (tState != heating){
				tState = heating;
				PORTC |= ((1 << 3) | (1 << 4) | (1 << 5));
				PORTC &= ~(1<<3);
			}
		}
		else if (farint > upperBound){
			if (tState != cooling){
				tState = cooling;
				PORTC |= ((1 << 3) | (1 << 4) | (1 << 5));
				PORTC &= ~(1<<5);
			}
		}
		else{
			if (tState != off){
				tState = off;
				PORTC |= ((1 << 3) | (1 << 4) | (1 << 5));
				PORTC &= ~(1<<4);
			}
		}
		//-------------------------LED OUTPUT-------------------------------LED OUTPUT--------------------
		//-------------------------LED OUTPUT-------------------------------LED OUTPUT--------------------
		//-------------------------SERVO OUTPUT-------------------------------SERVO OUTPUT--------------------
		//-------------------------SERVO OUTPUT-------------------------------SERVO OUTPUT--------------------
		//updates the position of the servomotor depending on what it should focus on
		if (servoState == temp){
			OCR2A = farint*4/10 - 5;
		}
		else if (servoState == lower){
			OCR2A = lowerBound*4/10 - 5;
		}
		else if (servoState == upper){
			OCR2A = upperBound*4/10 - 5;
		}
		//-------------------------SERVO OUTPUT-------------------------------SERVO OUTPUT--------------------
		//-------------------------SERVO OUTPUT-------------------------------SERVO OUTPUT--------------------
	}
}


//-------------------------BUTTON INPUT-------------------------------BUTTON INPUT--------------------
//-------------------------BUTTON INPUT-------------------------------BUTTON INPUT--------------------
char checkInputD(char bit)
{ 
  // will comment out for first test
  if (PIND & (1 << bit)) return 0;
  else return 1;
}
//-------------------------BUTTON INPUT-------------------------------BUTTON INPUT--------------------
//-------------------------BUTTON INPUT-------------------------------BUTTON INPUT--------------------




