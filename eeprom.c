
#include <htc.h>								// Processor definition file
#include "stdlib.h"								// standard library
#include "control.h"
#include "eeprom.h"
#include "IO.h"

//*****************************
//initial data stored in EEPROM.
//*****************************

// Control and Color data (used at the very first start)
__EEPROM_DATA(11, 0, 13, 88, 100, 0, 0, 0);				// (0)
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
__EEPROM_DATA(0, 20, 50, 255, 5, 0, 0, 255);			// (88) daylight, cool white
__EEPROM_DATA(50, 30, 0, 255, 70, 40, 0, 255);			// (96) warm white, warm white 2
__EEPROM_DATA(50, 100, 150, 0, 150, 100, 50, 0);		// (104)
__EEPROM_DATA(255, 255, 255, 255, 5, 5, 5, 5);			// (116)
__EEPROM_DATA(255, 255, 0, 255, 255, 255, 150, 255);	// (124)


/*
Reads 4 byte color data from EEPROM into color desigred struct
Memory Map:
Byte 0: R
Byte 1: G
Byte 2: B
Byte 4: W
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
Byte 4: W
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
write control data and colors to EEPROM. This is done when the power drops out, obviously the PIC needs some time to do this!!!
Memory Map:
Byte 0: control.mode
Byte 1: control.function
Byte 2: control.color_button (not really neccesary...)
Byte 3: control.eepointer (lowbyte)
Byte 4: control.brighntess_factor (lowbyte)
Byte 8: color.red
Byte 9: color.green
Byte 10: color.blue
Byte 11: color.white
*/
void write_eeprom(void)
{	
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
increments switch-on counter by one. can be read with pickit
we can start processing where we stopped before power off 
*/
void load_eeprom(void)
{
	char temp;
	int counter;
	temp = eeprom_read(EE_RUNDATA + 0);
	control.mode = temp;
	temp = eeprom_read(EE_RUNDATA + 1);
	control.function = temp;
	temp = eeprom_read(EE_RUNDATA + 2);
	control.color_button = temp;
	temp = eeprom_read(EE_RUNDATA + 3);
	control.eepointer = temp;
	temp = eeprom_read(EE_RUNDATA + 4);
	control.brightness_factor = temp;
	temp = eeprom_read(EE_RUNDATA + 8);
	color.red = temp;
	temp = eeprom_read(EE_RUNDATA + 9);
	color.green = temp;
	temp = eeprom_read(EE_RUNDATA + 10);
	color.blue = temp;
	temp = eeprom_read(EE_RUNDATA + 11);
	color.white = temp;
	//read counter
	counter = eeprom_read(EE_COUNT + 0);
	counter = counter << 8;
	temp = eeprom_read(EE_COUNT + 1);
	counter = counter + temp;
	counter++;
	//write counter  (working???)
	temp = counter;
	eeprom_write((EE_COUNT + 1), temp);
	temp = counter >> 8;
	eeprom_write((EE_COUNT + 0), temp);
}
