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

static void program_color(void);
static char set_dim_speed(char character);
static char set_powerup_mode(char character);


//globals
static char written;

/*
programming works as state machine. we start at state 1 and with every press of the program button, 
we do one initialion state, then we go one state further. The last step then sets back to state 1.
*/
void program(void)
{
	static char state;
	static char character = SD;
	//if cancel is pressed, exit program mode. This is done by going to the "finish" state
	if (rc5.command == CMD_CANCEL)
	{
		state = 24;
	}
	//if program button is pressed, go one step further and delete command
	if (rc5.command == CMD_PROGRAM)
	{
		state++;
		rc5.command = 0xff;
	}	
	//what is programmed depends on the momentary state. state 0 is not possible.
	//omitting the break commands in prepare cases causes that the next case is executed immediately 
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
			program_color();
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
			program_color();
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
			program_color();
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
			program_color();
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
			program_color();
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
			program_color();
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
			program_color();
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
			program_color();
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
			program_color();
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
			program_color();
			break;
		}
		//prepare for next programming state
		case 20:
		{
			//reset LED
			LED_MODE = 0;
			//clear written
			written = NO;
			//Set character to dim speed
			character = SD;
			//and go to next state
			state ++;
		}
		//set up speed delay in dim mode. 
		case 21:
		{
			character = set_dim_speed(character);
			lookup(character);
			break;
		}
		//prepare for next programming state
		case 22:
		{
			//reset LED
			LED_MODE = 0;
			//clear written
			written = NO;
			//set character to power up
			character = SP;
			//and go to next state
			state ++;
		}
		//we support 4 startup modes: Stored colors + stored brightness; stored colors + maximum brightness;
		//load color 10 + stored brightness; load color 10 + maximum brightness. in the last 2 modes, startup is always MANUAL!
		case 23:
		{
			character = set_powerup_mode(character);
			lookup(character);  
			break; 
		}
		//reset values to start. when changing the case number, also edit "cancel"
		case 24:
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
uses global written => is write process already done?
*/
static void program_color(void)
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
}

/*
sets fading speed in dim mode. every value between 1..255 is possible, 0 will cause an error and 255 will be veeeery ssslllooowww
gets: character that is momentary displayed
returns: character to display
*/
static char set_dim_speed(char character)
{
	//when right is pressed, decrease dim speed
	if (rc5.command == CMD_RIGHT)
	{
		control.dim_mode_speed++;
		//limit delay to 24, it's very slow and we don't have more different characters
		if (control.dim_mode_speed > 24)
		{
			control.dim_mode_speed = 1;
		}
		character = control.dim_mode_speed;
		//delete command
		rc5.command = 0xff;
	}
	//when left is pressed, increase dim speed
	if (rc5.command == CMD_LEFT)
	{
		control.dim_mode_speed--;
		//prevent 0
		if (control.dim_mode_speed < 1)
		{
			control.dim_mode_speed = 24;
		}
		character = control.dim_mode_speed;	
		//delete command
		rc5.command = 0xff;	
	}
	//test if OK is pressed long (== FAST) => store in Memory
	if ((rc5.command == CMD_OK) && (rc5.speed == FAST) && (written == NO))
	{
		write_byte(EE_RUNDATA + 5, control.dim_mode_speed);
		//prevent to execute this subroutine multiple times
		written = YES;
		//show write process is done
		LED_MODE = 1;
	}
	return character;
}

/*
sets power up mode. we support 4 startup modes: Stored colors + stored brightness; stored colors + maximum brightness;
load color 10 + stored brightness; load color 10 + maximum brightness. in the last 2 modes, startup is always MANUAL!
gets: character that is momentary displayed
returns: character to display
*/
static char set_powerup_mode(char character)
{
	//when right is pressed, go down
	if (rc5.command == CMD_RIGHT)
	{
		control.power_up_mode++;
		//limit to 3
		if (control.power_up_mode > 4)
		{
			control.power_up_mode = 1;
		}
		character = control.power_up_mode;
		//delete command
		rc5.command = 0xff;
	}
	//when left is pressed, go up
	if (rc5.command == CMD_LEFT)
	{
		control.power_up_mode--;
		//prevent neg.
		if (control.power_up_mode < 1)
		{
			control.power_up_mode = 4;
		}
		character = control.power_up_mode;	
		//delete command
		rc5.command = 0xff;	
	}
	//test if OK is pressed long (== FAST) => store in Memory
	if ((rc5.command == CMD_OK) && (rc5.speed == FAST) && (written == NO))
	{
		write_byte(EE_RUNDATA + 6, control.power_up_mode);
		//prevent to execute this subroutine multiple times
		written = YES;
		//show write process is done
		LED_MODE = 1;
	}
	return character;
}