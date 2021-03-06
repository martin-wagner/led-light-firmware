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

#include "control.h"
#include <stdlib.h>
#include "remote.h"
#include "htc.h"
#include "eeprom.h"
#include "IO.h"
#include "sevenseg.h"

static void dim_color(char *pupdown, int *pwait, int *pcolor);
static void bright_color(int *pcolor);
static void bright_factor(char *pfactor);
static void fade_color(int *value, int *desigred, char count_value);

//time to display something on the 7seg
static char display_on;

//direction for dim mode
static char updown1, updown2, updown3;

// delay time for color fading
#define COUNT_MAX 150

/*
Checks rc5.command to decide which mode is selected (dim, manual or program). The ELSE-IF is necessary as otherwise if 
the first IF is true, it makes the second one also true
*/
void set_mode(void)
{
	char command, test;

	command = rc5.command;
	//enable program mode. can only be exited by switching off power or going trough
	if (command == CMD_PROGRAM)
	{
		control.mode = PROGRAM;
	}
	// enable dim mode if current mode is manual and OK button is pressed.
	else if ((command == CMD_OK) && (control.mode == MANUAL))
	{
		control.mode = DIM;
		display_on = 30;
		//timer1 here is used to create radom numbers
		TMR1ON = 1;
		//set dim start modes
		if (color.red < 512)
		{
			updown1 = 1;
		}
		else
		{
			updown1 = 0;
		}
		if (color.green < 512)
		{
			updown2 = 1;
		}
		else
		{
			updown2 = 0;
		}
		if (color.blue < 512)
		{
			updown3 = 1;
		}
		else
		{
			updown3 = 0;
		}
	}
	else
	{
		// enable manual mode if current mode is dim and one of the following buttons is pressed:
		// OK, Power, a Color Button, Program +-. All other buttons are ignored
		// todo: geht das wirklich nur so??
		test = ((command == CMD_OK) || (command == CMD_POWER) || (command == CMD_RED) || (command == CMD_YELLOW) || 
				(command == CMD_BLUE) || (command == CMD_DARKORANGE) || (command == CMD_GREEN) || (command == CMD_PURPLE) 
				|| (command == CMD_ORANGE) || (command == CMD_TURQUOISE) || (command == CMD_PINK) || (command == CMD_WARMWHITE) 
				|| (command == CMD_COOLWHITE) || (command == CMD_DAYLIGHT) || (command == CMD_PROGRAMDEC) || (command == CMD_PROGRAMINC));
		test = (test && (control.mode == DIM));
		if (test == 1)
		{
			control.mode = MANUAL;
			//when quit dim mode
			//switch timer off and clear it
			TMR1ON = 0;
			TMR1 = 0;			
		}
	}
}


/*
Checks rc5.command to decide which function is going to be executed while in manual mode.
*/
void set_function(void)
{
	char command, test;

	//function selection only makes sense while in manual mode
	if (control.mode == MANUAL)
	{
		command = rc5.command;
		// some functions are not essential as they are interconnected with only one button, but it's easier this way
		// Power button function
		if (command == CMD_POWER)
		{
			control.function = FUNC_POWER;
		}
		// color button function
		test = ((command == CMD_RED) || (command == CMD_YELLOW) || (command == CMD_BLUE) || (command == CMD_DARKORANGE) || 
				(command == CMD_GREEN) || (command == CMD_PURPLE) || (command == CMD_ORANGE) || (command == CMD_TURQUOISE) 
				|| (command == CMD_PINK) || (command == CMD_WARMWHITE) || (command == CMD_COOLWHITE) || (command == CMD_DAYLIGHT));
		if (test == 1)
		{
			control.function = FUNC_COLOR;
		}
		// inc/dec color brightness buttons
		test = ((command == CMD_LEFT) || (command == CMD_RIGHT));
		if (test == 1)
		{
			control.function = FUNC_COLOR_SET;
		}
		// +/- Program Buttons
		test = ((command == CMD_PROGRAMDEC) || (command == CMD_PROGRAMINC));
		if (test == 1)
		{
			control.function = FUNC_PROGRAM;
		}
		// color on/off button
		if (command == CMD_COLORSONOFF)
		{
			control.function = FUNC_COLORSONOFF;
		}
		// this fuction doesn't have a user visible function, it only stores which color button has been pressed last
		test = ((command == CMD_REDBUTTON) || (command == CMD_GREENBUTTON) || (command == CMD_WHITEBUTTON) || (command == CMD_BLUEBUTTON));
		if (test == 1)
		{
			control.function = FUNC_COLOR_SELECT;
		}
		// white on/off button
		if (command == CMD_WHITEONOFF)
		{
			control.function = FUNC_WHITEONOFF;
		}	
	}
}

/*
Checks rc5.command to decide if we want to set the overall brightness. This is indipendent to the modes
*/
void set_brightness(void)
{
	char command, test;
	command = rc5.command;
	// brightness left/right enables brightness mode
	test = ((command == CMD_BRIGHTINC) || (command == CMD_BRIGHTDEC));
	if (test == 1)
	{
		control.brightness_set = BRIGHT_SET;
	}
}

/*
Starts dim mode. 
- Dims off white color (not used in dim mode)
*/
void mode_dim(void)
{
	static char i1, i2, i3;
	static int wait1, wait2, wait3;
	//dim red
	if (TMR2IF == 1)							// if timer2 postscaler flag is set
	{
		TMR2IF = 0;								// clear it
		/*nothing to do with red, but doing it here saves a timer
		this slowly dims off white when starting dim mode
		*/
		if (color.white	!= 0)
		{
			color.white--;
		}
		//this shows letter "d" for a short time to indicate dim mode
		if (display_on != 0)
		{
			display_on --;
			lookup(SD);
		}
		else
		{
			lookup(S0);
		}
		//slow down dim speed as set in programming mode
		if (i1 == control.dim_mode_speed)
		{		
			//call dim function				
			dim_color(&updown1, &wait1, &color.red);	
			i1 = 0;
		}
		i1 ++;
	}
	//dim led green
	if (TMR4IF == 1)
	{											// if timer4 postscaler flag is set
		TMR4IF = 0;								// clear it	
		//slow down dim mode as set while programming
		if (i2 == control.dim_mode_speed)
		{	
			//call dim function
			dim_color(&updown2, &wait2, &color.green);
			i2 = 0;
		}	
		i2++;
	}
	//dim led blue
	if (TMR6IF == 1)							// if timer6 postscaler flag is set
	{		
		TMR6IF = 0;								// clear it and call dim function
		//slow down dim mode as set while programming
		if (i3 == control.dim_mode_speed)
		{	
			//call dim funtion
			dim_color(&updown3, &wait3, &color.blue);
			i3 = 0;
		}
		i3++;
	}
}

/*
dim works as state machine => dim up => remain there for random time => dim down => remain there for random time =|
						   |______________________________________________________________________________________|
gets: pointers
*/
static void dim_color(char *pupdown, int *pwait, int *pcolor)
{
	char i;
	switch (*pupdown)						// test if brightness should be increased =0 or decreased =1 or should remain constant >1
	{	
		case (0) :					
		{		
			if (*pcolor >= 0x3ff)			// when maximum brightness is reached, set flag to start waiting time
			{
					*pupdown = 2;
			}
			else
			{
				(*pcolor) ++;
			}
			break;
		}	
		case (1) :
		{
			if (*pcolor <= 0)				// when minimum brightness is reached, clear flag to start waiting time
			{
				*pupdown = 3;
			}
			else
			{
				(*pcolor) --;
			}
			break;
		}
		case (2) :
		{
			//state2 is not used, so no waiting time at maximum brightness. creation of waiting time has to be interchanged when using this
			//*pwait = *pwait - 1;				// block until wait == 0
		//	if (*pwait == 0)					// returns to dimming operation and resets waiting timer
		//	{
				*pupdown = 1;
			//	*pwait = rand();				// creates random number
			//	*pwait = *pwait & 0b00111111;	// limit random number to 64
		//	}
			break;
		}
		case (3) :
		{
			*pwait = *pwait - 1;				// block until wait == 0
			if (*pwait <= 0)					// returns to dimming operation and resets waiting timer
			{
				*pupdown = 0;
				// random number is at least 200
				do
				{
					//random delay if wait < 200. At first loop run, pwait should be 0
					for (i = 0; i <= *pwait; i++);
				//	*pwait = rand();			// creates random number
					*pwait = TMR1;				// reading timer value at random time instead of rand() function saves 5% program memory
					*pwait = *pwait & 0x7ff;	// maximum number is 2047
				}
				while (*pwait < 200);
			}
			break;
		}
	}

	// as color brightness and off time is set randomly, it is possilble that all colors are going off. In this case we start dimming up instantly
	if ((color.red + color.green + color.blue) < 30)
	{
		*pcolor = *pcolor + 2;					// next color can make an dec without being set to dim up
		*pupdown = 0;
	}
	// try to prevent color getting too white
	else if ((color.red + color.green + color.blue) > 2300)
	{
		*pcolor = *pcolor - 2;					// next color can make an inc without being set to dim down
		*pupdown = 1;
	}
}

/*
Switches on/off colors. When off, values are stored, when back on, they are restored.
If we have left off mode since switching off, the momentary color values are stored (function has to be called a 2nd time to do this)
and the old ones are lost.
*/
void onoff(void)
{
	static bit toggle;
	static int red, green, blue, white;
	// if power on, store momentary values and dim them off.
	if (toggle == 0)
	{
		red = color.red;
		green = color.green;
		blue = color.blue;
		white = color.white;
		color_desigred.red = 0;
		color_desigred.green = 0;
		color_desigred.blue = 0;
		color_desigred.white = 0;
		control.function = FUNC_FADING;
	}
	// if power off, restore values.
	else
	{
		// test if we are still in off mode. If not, the main program has to call onoff again to store momentary 
		// values (therefore fading is only set when one if statement is processed, otherwise it remains at colorsonoff)
		if ((color.red || color.green || color.blue || color.white) == 0)
		{
			color_desigred.red = red;
			color_desigred.green = green;
			color_desigred.blue = blue;
			color_desigred.white = white;
			control.function = FUNC_FADING;
		}
	}
	toggle = ~toggle;
}

/*
sets color selected by button press
*/
void func_color(void)
{
	char sel_color;
	sel_color = rc5.command;
	switch (sel_color)
	{
		case CMD_RED:
		{
			read_color(EE_RED);
			break;
		}
		case CMD_YELLOW:
		{
			read_color(EE_YELLOW);			
			break;
		}
		case CMD_BLUE:
		{
			read_color(EE_BLUE);		
			break;
		}
		case CMD_DARKORANGE:
		{
			read_color(EE_DARKORANGE);			
			break;
		}
		case CMD_GREEN:
		{
			read_color(EE_GREEN);			
			break;
		}
		case CMD_PURPLE:
		{
			read_color(EE_PURPLE);		
			break;
		}
		case CMD_ORANGE:
		{
			read_color(EE_ORANGE);			
			break;
		}
		case CMD_TURQUOISE:
		{
			read_color(EE_TURQUOISE);			
			break;
		}
		case CMD_PINK:
		{
			read_color(EE_PINK);		
			break;
		}
		case CMD_WARMWHITE:
		{
			read_color(EE_WARMWHITE);	
			break;
		}
		case CMD_COOLWHITE:
		{
			read_color(EE_COOLWHITE);
			break;
		}
		case CMD_DAYLIGHT:
		{
			read_color(EE_DAYLIGHT);		
			break;
		}
	}
	control.function = FUNC_FADING;
}

/*
increses/decreases with colorbutton selected color brightness
*/
void func_color_set(void)
{
	char color_button;
	int *red_p, *green_p, *blue_p, *white_p;
	color_button = control.color_button;
	switch (color_button)
	{
		case (CMD_REDBUTTON):
		{
			red_p = &color.red;
			bright_color(red_p);
			break;
		}
		case (CMD_GREENBUTTON):
		{
			green_p = &color.green;
			bright_color(green_p);
			break;
		}
		case (CMD_BLUEBUTTON):
		{
			blue_p = &color.blue;
			bright_color(blue_p);
			break;
		}
		case (CMD_WHITEBUTTON):
		{
			white_p = &color.white;
			bright_color(white_p);
			break;
		}
	}
	control.function = IDLE;	
}

/*
increses/decreases brightness of in pointer selected color until the limits are reached
- rc5.speed is to detect if the button is kept pressed
*/
static void bright_color(int *pcolor)
{
	int color;
	color = *pcolor;
	// increase brightness
	if (rc5.command == CMD_RIGHT)
	{
		if (rc5.speed == FAST)
		{	
			color = color + 25;
		}
		else
		{
			color++;
		}
	}
	// decrease brightness
	if (rc5.command == CMD_LEFT)
	{
		if (rc5.speed == FAST)
		{	
			color = color - 25;
		}
		else
		{
			color--;
		}
	}
	// fit computed result back into limits
	if (color < 0)
	{
		color = 0;
	}
	if (color > 0x3ff)
	{
		color = 0x3ff;
	}
	*pcolor = color;
}

void fucn_program(void)
{
	// load programmed colors from eeprom. 
	// Program memory works as ring memory
	if (rc5.command == CMD_PROGRAMINC)
	{
		control.eepointer = control.eepointer + 8;
		if (control.eepointer > EE_MAXPROG)
		{
			control.eepointer = EE_MINPROG;
		}		
	}
	if (rc5.command == CMD_PROGRAMDEC)
	{
		control.eepointer = control.eepointer - 8;
		if (control.eepointer < EE_MINPROG)
		{
			control.eepointer = EE_MAXPROG;
		}		
	}
	read_color(control.eepointer);	
	control.function = FUNC_FADING;
}

/*
Switches on/off colors. When off, color values are stored, when back on, they are restored.
If we have left colors off since switching off, the momentary color values are stored (function has to be called a 2nd time to do this)
and the old ones are lost.
*/
void fuc_colorsonoff(void)
{
	static bit toggle;
	static int red, green, blue;
	// if colors are on now, store momentary values and dim them off. this musn't change white!
	if (toggle == 0)
	{
		red = color.red;
		green = color.green;
		blue = color.blue;
		color_desigred.red = 0;
		color_desigred.green = 0;
		color_desigred.blue = 0;
		color_desigred.white = color.white;
		control.function = FUNC_FADING;
	}
	// if colors are off, restore color values. this also musn't change white!
	else
	{
		// test if we are still in color off mode. If not, the main program has to call colorsonoff again to store momentary 
		// color values (therefore fading is only set when one if statement is processed, otherwise it remains at colorsonoff)
		if ((color.red == 0) && (color.green == 0) && (color.blue == 0))
		{
			color_desigred.red = red;
			color_desigred.green = green;
			color_desigred.blue = blue;
			color_desigred.white = color.white;
			control.function = FUNC_FADING;
		}
	}
	toggle = ~toggle;
}

/*
Switches on/off white. Works the same way as colors
*/
void fuc_whiteonoff(void)
{
	static bit toggle;
	static int white;
	// if white is on now, store momentary value and dim it off. this musn't change colors!
	if (toggle == 0)
	{
		white = color.white;
		color_desigred.red = color.red;
		color_desigred.green = color.green;
		color_desigred.blue = color.blue;
		color_desigred.white = 0;
		control.function = FUNC_FADING;
	}
	// if white is off, restore value. this also musn't change colors!
	else
	{
		// test if we are still in white off mode. If not, the main program has to call whiteonoff again to store momentary 
		// white value (therefore fading is only set when one if statement is processed, otherwise it remains at whiteonoff)
		if (color.white == 0)
		{
			color_desigred.red = color.red;
			color_desigred.green = color.green;
			color_desigred.blue = color.blue;
			color_desigred.white = white;
			control.function = FUNC_FADING;
		}
	}
	toggle = ~toggle;
}

/*
stores which of the colored buttons on the remote has been pressed last. This is necessary for brightness function
*/
void func_colorselect(void)
{
	control.color_button = rc5.command;
	control.function = IDLE;
}

/*
fades color from momentary value to desigred value
*/
void func_fadecolor(void)
{
	unsigned int tmr1_value;
	char count_value;

	//here timer1 is used as timer
	TMR1ON = 1;				
	if (TMR1 > COUNT_MAX)
	{
		//as timer1 exceeds it's max value most of the times, we need to know how many times
		//it did so we can compensate it by adding an equivalent value
		tmr1_value = TMR1;
		TMR1 = 0;
		count_value = tmr1_value / COUNT_MAX + 1;		//always round up
		//fade colors
		fade_color(&color.red, &color_desigred.red, count_value);
		fade_color(&color.green, &color_desigred.green, count_value);
		fade_color(&color.blue, &color_desigred.blue, count_value);
		fade_color(&color.white, &color_desigred.white, count_value);
	}
	//when all colors are as desigred, switch off timer and go back to idle
	if ((color.red == color_desigred.red) && (color.green == color_desigred.green) && 
	   (color.blue == color_desigred.blue) && (color.white == color_desigred.white))
	{
		TMR1ON = 0;
		control.function = IDLE;
	}
}

/*
fades single color up or down to desigred color value by count_value
*/
static void fade_color(int *value, int *desigred, char count_value)
{
	// we have to dim up or down a color to get to the new brightness. nothing is done if equal.
	if (*value > *desigred)
	{
		*value = *value - count_value;
		//check for overshoot
		if (*value < *desigred)
		{
			*value = *desigred;
		}
	}
	if (*value < *desigred)
	{
		*value = *value + count_value;
		//check for overshoot
		if (*value > *desigred)
		{
			*value = *desigred;
		}
	}
}

/*
increses/decreases overall color brightness
*/
void brightness(void)
{
	char *brightness_factor_p;
	brightness_factor_p = &control.brightness_factor;
	//set new brightness
	bright_factor(brightness_factor_p);
}

/*
works exactly the same as bright_color
increses/decreases the brightness factor until the limits are reached
- integer factor is used to detect under/overflows of the char *pfactor
- rc5.speed is to detect if the button is kept pressed
- brightness is only 8 bit wide
*/
static void bright_factor(char *pfactor)
{
	int factor;
	factor = *pfactor;
	// increase brightness
	if (rc5.command == CMD_BRIGHTINC)
	{
		if (rc5.speed == FAST)
		{	
			factor = factor + 16;
		}
		else
		{
			factor++;
		}
	}
	// decrease brightness
	if (rc5.command == CMD_BRIGHTDEC)
	{
		if (rc5.speed == FAST)
		{	
			factor = factor - 16;
		}
		else
		{
			factor--;
		}
	}
	// fit computed result back into char limits. By using "5", dimming "off" is not possible.
	if (factor < 5)
	{
		factor = 5;
	}
	if (factor > 0xff)
	{
		factor = 0xff;
	}
	*pfactor = factor;
}

