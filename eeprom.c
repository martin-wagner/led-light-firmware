
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
__EEPROM_DATA(MANUAL, 0, 13, EE_MINPROG, 255, 3, 1, 0);	// (0)
__EEPROM_DATA(255, 127, 63, 32, 0, 0, 0, 0);			// (8)		
// Reserve
__EEPROM_DATA(0, 0, 0, 0, 0, 0, 0, 0);					// (16)
__EEPROM_DATA(0, 0, 0, 0, 0, 0, 0, 0);					// (24)
__EEPROM_DATA(0, 0, 0, 0, 0, 0, 0, 0);					// (32)

// colors  	(   R  G  B  W  R   G   B  W )				// block start adress
__EEPROM_DATA(255, 8, 0, 3, 175, 255, 0, 5);			// (40) red, yellow 
__EEPROM_DATA(0, 0, 255, 0, 255, 150, 2, 5);			// (48) blue, darkorange
__EEPROM_DATA(9, 255, 2, 3, 80, 0, 150, 2);				// (56) green, purple
__EEPROM_DATA(208, 160, 0, 3, 0, 255, 48, 3);			// (64) orange, turquoise
__EEPROM_DATA(180, 0, 180, 13, 192, 192, 0, 255);		// (72) pink, warm white			
__EEPROM_DATA(32, 32, 0, 255, 16, 80, 112, 255);		// (80) cool white, daylight
// programmable colors (defaults)
//   		( R    G  B   W   bright), brightness is not used at the moment
__EEPROM_DATA(255, 8, 0, 60, 255, 0, 0, 0);				// (88) bright red
__EEPROM_DATA(0, 30, 255, 40, 255, 0, 0, 0);			// (96) bright blue
__EEPROM_DATA(9, 255, 2, 40, 255, 0, 0, 0);				// (104) bright green
__EEPROM_DATA(0, 255, 48, 15, 255, 0, 0, 0);			// (112) bright turquoise
__EEPROM_DATA(40, 50, 0, 0, 255, 0, 0, 0);				// (120) dark yellow
__EEPROM_DATA(5, 50, 0, 0, 255, 0, 0, 0);				// (128) dark green
__EEPROM_DATA(50, 5, 0, 0, 255, 0, 0, 0);				// (136) dark red
__EEPROM_DATA(0, 0, 25, 0, 255, 0, 0, 0);	    		// (144) dark blue
__EEPROM_DATA(255, 255, 255, 255, 255, 0, 0, 0);		// (152) all on
__EEPROM_DATA(130, 130, 0, 255, 255, 0, 0, 0);			// (160) startup color 

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
ALL colors are processed as 10 Bit values but only stored in EEPROM as 8 bit values!!!!
therefore when loading, the read value is rotated to the left by two and when writing two to the rigth.
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
void read_color(char addr)
{
	int temp;
	temp = eeprom_read(addr + 0);
	temp = temp << 2;
	color_desigred.red = temp;
	temp = eeprom_read(addr + 1);
	temp = temp << 2;
	color_desigred.green = temp;
	temp = eeprom_read(addr + 2);
	temp = temp << 2;
	color_desigred.blue = temp;
	temp = eeprom_read(addr + 3);
	temp = temp << 2;
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
void write_color(char addr)
{
	char temp;
	temp = color.red >> 2;
	eeprom_write((addr + 0), temp);
	temp = color.green >> 2;
	eeprom_write((addr + 1), temp);
	temp = color.blue >> 2;
	eeprom_write((addr + 2), temp);
	temp = color.white >> 2;
	eeprom_write((addr + 3), temp);
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
increments switch-on counter by one. can be read with pickit. we use 24 bit, so the eeprom will quit working before we overflow
*/
void write_eeprom(void)
{
	unsigned int temp;
	unsigned short long int counter;	
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
	write_color(EE_RUNDATA + 8);
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

/* 
restores data written at power off from the EEPROM
we can start processing where we stopped before power off 
*/
void load_eeprom(void)
{
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
			read_color(EE_RUNDATA + 8);
			//read color reads to color_desigred but here we need it directly
			color = color_desigred;
			break;
		}
		//same as above, mode = MANUAL, load color 10
		case 4:
		{				
			control.brightness_factor = 0xff;	
		}
		case 3:
		{
			control.mode = MANUAL;
			control.function = IDLE;
			read_color(EE_PROG9);
			color = color_desigred;
			break;
		}	
	}
	//check if we have all LEDs switched off in loaded color for any reason
	if ((color.red || color.green || color.blue || color.white) == 0)
	{
		color.white = 100;
	}
}
