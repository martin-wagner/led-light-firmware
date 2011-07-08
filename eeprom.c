
#include <htc.h>								// Processor definition file
#include "stdlib.h"								// standard library
#include "control.h"
#include "eeprom.h"
#include "IO.h"
#include "remote.h"

//*****************************
//initial data stored in EEPROM.
//*****************************

// Control and Color data (used at the very first start)
__EEPROM_DATA(DIM, 0, 13, EE_MINPROG, 255, 3, 1, 0);	// (0)
__EEPROM_DATA(255, 127, 63, 32, 0, 0, 0, 0);			// (8)		
// Reserve
__EEPROM_DATA(0, 0, 0, 0, 0, 0, 0, 0);					// (16)
__EEPROM_DATA(0, 0, 0, 0, 0, 0, 0, 0);					// (24)
__EEPROM_DATA(0, 0, 0, 0, 0, 0, 0, 0);					// (32)

// colors  	(   R  G  B  W  R   G   B  W )				// block start adress
__EEPROM_DATA(255, 7, 0, 5, 2, 255, 2, 5);				// (40) red, green
__EEPROM_DATA(3, 3, 150, 0, 180, 20, 0, 0);				// (48) darkblue, darkorange
__EEPROM_DATA(1, 150, 1, 0, 80, 0, 150, 2);				// (56) darkgreen, purple
__EEPROM_DATA(255, 50, 0, 5, 0, 255, 100, 2);			// (64) orange, turquoise
__EEPROM_DATA(180, 0, 180, 30, 255, 200, 2, 5);			// (72) pink, yellow			
__EEPROM_DATA(0, 30, 255, 30, 100, 50, 50, 0);			// (80) blue, brown
// programmable colors (defaults)
//   		( R  G    B   W   bright), brightness is not used at the moment
__EEPROM_DATA(0, 20, 50, 255, 255, 0, 0, 0);			// (88) daylight
__EEPROM_DATA(5, 0, 0, 255, 255, 0, 0, 0);				// (96) cool white
__EEPROM_DATA(50, 30, 0, 255, 255, 0, 0, 0);			// (104)warm white 
__EEPROM_DATA(70, 40, 0, 255, 255, 0, 0, 0);			// (112)warm white 2
__EEPROM_DATA(100, 50, 0, 255, 50, 0, 0, 0);			// (120)
__EEPROM_DATA(63, 63, 63, 63, 255, 0, 0, 0);			// (128)
__EEPROM_DATA(127, 127, 127, 127, 255, 0, 0, 0);		// (136)
__EEPROM_DATA(255, 255, 255, 255, 75, 0, 0, 0);	    	// (144)
__EEPROM_DATA(255, 255, 255, 255, 175, 0, 0, 0);		// (152)
__EEPROM_DATA(255, 255, 255, 255, 255, 0, 0, 0);		// (160)

/*
//DO NOT CHANGE, for new use unused bytes!!!
Memory Map:
Byte 0: control.mode
Byte 1: control.function
Byte 2: control.color_button (not really neccesary...)
Byte 3: control.eepointer (lowbyte)
Byte 4: control.brighntess_factor (lowbyte)
Byte 5: control.dim_mode_speed (read at every start, written only in programming mode)
Byte 6: control.power_up_mode (read at every start, written only in programming mode)
Byte 8: color.red
Byte 9: color.green
Byte 10: color.blue
Byte 11: color.white
*/



/*
Reads 4 byte color data from EEPROM into color desigred struct
Memory Map:
Byte 0: R
Byte 1: G
Byte 2: B
Byte 3: W
Gets: Read start address
*/
void read_color(char adr)
{
	char temp;
	temp = eeprom_read(adr + 0);
	color_desigred.red = temp;
	temp = eeprom_read(adr + 1);
	color_desigred.green = temp;
	temp = eeprom_read(adr + 2);
	color_desigred.blue = temp;
	temp = eeprom_read(adr + 3);
	color_desigred.white = temp;
}

/*
Writes 4 byte color data to EEPROM from color struct
Memory Map:
Byte 0: R
Byte 1: G
Byte 2: B
Byte 3: W
Gets: Write start address
*/
void write_color(char adr)
{
	eeprom_write((adr + 0), color.red);
	eeprom_write((adr + 1), color.green);
	eeprom_write((adr + 2), color.blue);
	eeprom_write((adr + 3), color.white);
}


/*
Writes 1 byte to eeprom. this routine is not essential, but eeprom_write is now called only from eeprom file
Gets: Write start address, byte to write
*/
void write_byte(char addr, char byte)
{
	eeprom_write(addr, byte);
}

/*
write control data and colors to EEPROM. This is done when the power drops out, obviously the PIC needs some time to do this!!!
*/
void write_eeprom(void)
{	
	//we musn't resume in program mode
	if (control.mode == PROGRAM)
	{
		control.mode = MANUAL;
	}
	eeprom_write((EE_RUNDATA + 0), control.mode);
	eeprom_write((EE_RUNDATA + 1), control.function);
	eeprom_write((EE_RUNDATA + 2), control.color_button);
	eeprom_write((EE_RUNDATA + 3), control.eepointer);
	eeprom_write((EE_RUNDATA + 4), control.brightness_factor);
	eeprom_write((EE_RUNDATA + 8), color.red);
	eeprom_write((EE_RUNDATA + 9), color.green);
	eeprom_write((EE_RUNDATA + 10), color.blue);
	eeprom_write((EE_RUNDATA + 11), color.white);
}

/* 
restores data written at power off from the EEPROM
increments switch-on counter by one. can be read with pickit. we use 24 bit, so the eeprom will quit before we overflow
we can start processing where we stopped before power off 
*/
void load_eeprom(void)
{
	unsigned int temp;
	unsigned short long int counter;
	control.color_button = eeprom_read(EE_RUNDATA + 2);
	control.eepointer = eeprom_read(EE_RUNDATA + 3);
	control.brightness_factor = eeprom_read(EE_RUNDATA + 4);
	control.dim_mode_speed = eeprom_read(EE_RUNDATA + 5);
	control.power_up_mode = eeprom_read(EE_RUNDATA + 6);
	//test which power up mode we are using
	switch (control.power_up_mode)
	{	
		//load color + max bright. no break, so case 1 is also executed to load colors and mode.
		case 2:
		{				
			control.brightness_factor = 0xff;	
		}
		//load colors and mode
		case 1:
		{
			control.mode = eeprom_read(EE_RUNDATA + 0);
			control.function = eeprom_read(EE_RUNDATA + 1);
			color.red = eeprom_read(EE_RUNDATA + 8);
			color.green = eeprom_read(EE_RUNDATA + 9);
			color.blue = eeprom_read(EE_RUNDATA + 10);
			color.white = eeprom_read(EE_RUNDATA + 11);
			break;
		}
		//same system as above, mode = MANUAL, load color 10
		case 4:
		{				
			control.brightness_factor = 0xff;	
		}
		case 3:
		{
			control.mode = MANUAL;
			control.function = IDLE;
			color.red = eeprom_read(EE_PROG9 + 0);
			color.green = eeprom_read(EE_PROG9 + 1);
			color.blue = eeprom_read(EE_PROG9 + 2);
			color.white = eeprom_read(EE_PROG9 + 3);
			break;
		}	
	}
	//read counter
	counter = eeprom_read(EE_COUNT + 0);
	counter = counter << 16;
	temp = eeprom_read(EE_COUNT + 1);
	counter = counter + (temp << 8);
	temp = eeprom_read(EE_COUNT + 2);
	counter = counter + temp;
	counter++;
	//write counter  (working???)
	temp = counter;
	eeprom_write((EE_COUNT + 2), (char)temp);
	temp = counter >> 8;
	eeprom_write((EE_COUNT + 1), (char)temp);
	temp = counter >> 16;
	eeprom_write((EE_COUNT + 0), (char)temp);
}
