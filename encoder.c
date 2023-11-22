#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "project.h"
#include "encoder.h"


void updateTemp(int x);

//-------------------------ENCODER INPUT-------------------------------ENCODER INPUT--------------------
//-------------------------ENCODER INPUT-------------------------------ENCODER INPUT--------------------
ISR(PCINT1_vect)
{
    // In Task 6, add code to read the encoder inputs and determine the new
    // count value
	x = PINC;
	a = (x & (1 << 1)) != 0;  
	b = (x & (1 << 2)) != 0;  
	if (old_state == 0) {
	    // Handle A and B inputs for state 0
		if (a == 1){
			new_state = 1;
			updateTemp(1);
		}
		else if (b ==1){
			new_state = 2;
			updateTemp(-1);
		}
	}
	else if (old_state == 1) {

	    // Handle A and B inputs for state 1
		if (b == 1){
			new_state = 3;
			updateTemp(1);
		}
		else if (a ==0){
			new_state = 0;
			updateTemp(-1);
		}
	}
	else if (old_state == 2) {
	    // Handle A and B inputs for state 2
		if (b == 0){
			new_state = 0;
			updateTemp(1);
		}
		else if (a ==1){
			new_state = 3;
			updateTemp(-1);
		}
	}
	else {   // old_state = 3
	    // Handle A and B inputs for state 3
		if (a == 0){
			new_state = 2;
			updateTemp(1);
		}
		else if (b ==0){
			new_state = 1;
			updateTemp(-1);
		}
	}

	if (new_state != old_state) {
	    tempChangeBool = 1;
	    old_state = new_state;
	}
}
//-------------------------ENCODER INPUT-------------------------------ENCODER INPUT--------------------
//-------------------------ENCODER INPUT-------------------------------ENCODER INPUT--------------------

//------------------ENCODER INPUT HELPER FUNCTION-------------ENCODER INPUT HELPER FUNCTION--------------------
//------------------ENCODER INPUT HELPER FUNCTION-------------ENCODER INPUT HELPER FUNCTION--------------------
void updateTemp(int x){
	if (state == setLow){
		lowerBound += x;
		if (lowerBound < 50) lowerBound = 50;
		if (lowerBound > upperBound) lowerBound = upperBound;
		eeprom_update_byte((void *) 1, lowerBound);
	}
	else if (state == setHigh){
		upperBound += x;
		if (upperBound > 90) upperBound = 90;
		if (upperBound < lowerBound) upperBound = lowerBound;
		eeprom_update_byte((void *) 2, upperBound);
	}
}
//------------------ENCODER INPUT HELPER FUNCTION-------------ENCODER INPUT HELPER FUNCTION--------------------
//------------------ENCODER INPUT HELPER FUNCTION-------------ENCODER INPUT HELPER FUNCTION--------------------
