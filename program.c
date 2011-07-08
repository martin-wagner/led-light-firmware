#include <htc.h>								// Processor definition file
#include "stdlib.h"								// standard library
#include "remote.h"
#include "control.h"
#include "eeprom.h"
#include "IO.h"
#include "program.h"
#include "sevenseg.h"

//to "written"
#define YES 1
#define NO 0

static char program_color(char written);


/*
programming works as state machine. we start at state 1 and with every press of the program button, 
we do one initialion state, then we go one state further. The last step then sets back to state 1.
*/
void program(void)
{
	static char state, written;
	static bit check;
	//if cancel is pressed, exit program mode. This is done by going to the "finish" state
	if (rc5.command == CMD_CANCEL)
	{
		state = 20;
	}
	//if program button is pressed, go one step further and delete command
	if (rc5.command == CMD_PROGRAM)
	{
		state++;
		rc5.command = 0xff;
	}	
	//what is programmed depends on the momentary state. state 0 is not possible.
	switch (state)
	{
		//program user color 1
		case 1:
		{
			//load first memory address
			control.eepointer = EE_MINPROG;
			//display
			lookup(1);
			//write output
			written = program_color(written);
			break;
		}
		//prepare for next programming state
		case 2:
		{
			//go to next memory address
			control.eepointer = control.eepointer + 8;	
			//clear written
			written = NO;
			//reset LED
			LED_MODE = 0;
			//and go to next state
			state ++;
		}
		//program user color 2
		case 3:
		{
			//display
			lookup(2);
			//write output
			written = program_color(written);
			break;
		}
		//prepare for next programming state
		case 4:
		{
			//go to next memory address
			control.eepointer = control.eepointer + 8;	
			//clear written
			written = NO;
			//reset LED
			LED_MODE = 0;
			//and go to next state
			state ++;
		}
		//program user color 3
		case 5:
		{
			//display
			lookup(3);
			//write output
			written = program_color(written);
			break;
		}
		//prepare for next programming state
		case 6:
		{
			//go to next memory address
			control.eepointer = control.eepointer + 8;	
			//clear written
			written = NO;
			//reset LED
			LED_MODE = 0;
			//and go to next state
			state ++;
		}
		//program user color 4
		case 7:
		{
			//display
			lookup(4);
			//write output
			written = program_color(written);
			break;
		}
		//prepare for next programming state
		case 8:
		{
			//go to next memory address
			control.eepointer = control.eepointer + 8;	
			//clear written
			written = NO;
			//reset LED
			LED_MODE = 0;
			//and go to next state
			state ++;
		}
		//program user color 5
		case 9:
		{
			//display
			lookup(5);
			//write output
			written = program_color(written);
			break;
		}
		//prepare for next programming state
		case 10:
		{
			//go to next memory address
			control.eepointer = control.eepointer + 8;	
			//clear written
			written = NO;
			//reset LED
			LED_MODE = 0;
			//and go to next state
			state ++;
		}
		//program user color 6
		case 11:
		{
			//display
			lookup(6);
			//write output
			written = program_color(written);
			break;
		}
		//prepare for next programming state
		case 12:
		{
			//go to next memory address
			control.eepointer = control.eepointer + 8;	
			//clear written
			written = NO;
			//reset LED
			LED_MODE = 0;
			//and go to next state
			state ++;
		}
		//program user color 7
		case 13:
		{
			//display
			lookup(7);
			//write output
			written = program_color(written);
			break;
		}
		//prepare for next programming state
		case 14:
		{
			//go to next memory address
			control.eepointer = control.eepointer + 8;	
			//clear written
			written = NO;
			//reset LED
			LED_MODE = 0;
			//and go to next state
			state ++;
		}		
		//program user color 8
		case 15:
		{
			//display
			lookup(8);
			//write output
			written = program_color(written);
			break;
		}
		//prepare for next programming state
		case 16:
		{
			//go to next memory address
			control.eepointer = control.eepointer + 8;	
			//clear written
			written = NO;
			//reset LED
			LED_MODE = 0;
			//and go to next state
			state ++;
		}
		//program user color 9
		case 17:
		{
			//display
			lookup(9);
			//write output
			written = program_color(written);
			break;
		}
		//prepare for next programming state
		case 18:
		{
			//go to next memory address
			control.eepointer = control.eepointer + 8;	
			//clear written
			written = NO;
			//reset LED
			LED_MODE = 0;
			//and go to next state
			state ++;
		}
		//program user color 10
		case 19:
		{
			//display
			lookup(0);
			//write output
			written = program_color(written);
			break;
		}
		//reset values to start
		case 20:
		{
			//clear display
			lookup(S0);
			//finish programming
			control.mode = MANUAL;
			control.function = IDLE;
			//reset pointer
			control.eepointer = EE_MINPROG;
			//reset state
			state = 0;
			//clear written
			written = NO;
			//reset LED
			LED_MODE = 0;
			break;
		}
	}	
}

/*
writes momentary color values to memory position selected in control.eepointer
gets: written => is write process already done?
returns: YES when write is processed
*/
static char program_color(char written)
{
	//test if OK is pressed long (== FAST) => store in Memory
	if ((rc5.command == CMD_OK) && (rc5.speed == FAST) && (written == NO))
	{
		write_color(control.eepointer);
		//prevent to execute this subroutine multiple times
		written = YES;
		//show write process is done
		LED_MODE = 1;
		
	}
	return written;	
}