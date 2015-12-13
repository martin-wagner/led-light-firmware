/*
    Electronic LED Light Firmware
    
    Copyright (C) 2015  Martin Wagner

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <htc.h>								// Processor definition file
#include "IO.h"
#include "remote.h"


/*
Interrupts are only used for the RC5 receiving procedure. Port B0 is used as edge detection input (falling edge). 
The corresponding ISR starts Timer0 and disables edge detection. Timer0 ISR then records 26 samples on B0, one every 
0.9 ms. The 27th call checks parity and enables edge detection again.
First RC5 Start Bit is not recorded, rc5_data contains 13 bits coded in manchester code (= 26 bit).
*/

void interrupt isr(void)
{
	//var
	static bit isr_sel, bit_read;
	static char n;
	static signed char parity;
	//this ISR detects the falling edge created by the IR sensor (RC5 Start Bit).
	isr_sel = INTE && INTF;						// test if interrupt is activated and requestet by INT Pin 
	isr_sel = isr_sel && ! rc5_ready;			// only start receiving a new frame if last one is processed

	if (isr_sel == 1)
	{
		LED_RECEIVE = 1;						// set receiving LED
		//set up parameters to start reading of the command frame sent by the remote.
		//timer0 delays until we are somewhere in the middle of the next rc5 half data bit
		TMR0 = 0x00;							
		TMR0IF = 0;								
		TMR0IE = 1;
		parity = 0;								// clear the parity bit
		rc5_data = 0;							// clear last rc5_datad frame
		n = 0;									// set index to 0 read bits
		INTE = 0;								// disable edge detection (must be activated again once receiving of the command is completed)	
	}
	//this ISR reads values from RB0, stores them and creates the parity. It doesn't interpret the manchester code, it just stores a bit every 0,9ms
	isr_sel = TMR0IE && TMR0IF;	
	if (isr_sel == 1)
	{
		//one RC5 frame in manchester coding is 26 bit long (start is not stored), therefore we rotate 26 bits (0...25) into rc5_data register 
		if (n < 26)
		{
			/* start new wait period. Delay time should be ~0,9 ms. todo: tweak here if remote doesn't work properly.
			 */
			TMR0 = 0x28;						
			bit_read = IR_IN;					// store the read bit in memory to be independend from changes on the actual port pin
			//create parity. As manchester code is DC free, after one complete frame result must be 0. Otherwise frame invalid
			if (bit_read == 1)
			{
				parity ++;
			}
			else
			{
				parity --;
			}
			rc5_data = rc5_data << 1;			// rotate bit in c doesn't work as in assembler... first rotate left by one (in first run it's a 0)
			rc5_data = rc5_data + bit_read;		// then "add" the value of bit_read	

			n++;								// increment cycle counter by one	
			
		}
		//when we have our 26 bits, finish reading, check parity and set parameters for edge detection again
		else
		{							
			TMR0IE = 0;							// disable timer interrupt
			INTF = 0;							// enable edge detection
			INTE = 1;	
			// only if parity is ok, enable access in main program
			if (parity == 0)					
			{
				rc5_ready = 1;						
			}
			LED_RECEIVE = 0;					//clear receiving LED
		}
	}
	//reset interrupt requests
	TMR0IF = 0;
	INTF = 0;						
	ei();
}
