#include <htc.h>								// Processor definition file
#include "remote.h"
#include "IO.h"

volatile long unsigned int rc5_data;
volatile char rc5_ready;


/*
converts the received manchester coded rc5 frame into individual bytes for toggle, device id and command
returns 1 when error occured during conversation, otherwise 0
*/
bit get_rc5_control(void)
{
	//var
	char n, device, command, bit_man;
	static bit bit_buffer, error;
	unsigned int buffer;
	error = 0;	
	//decode manchestercode by rotating 2 manchester bits == 1 bit out of received frame, flush start bit	
	//note that direction of bits is interchanged during this process!!!
	buffer = 0;
	for (n = 0; n < 12; n++)
	{
		bit_man = 0;
		//load 2 bits from rc5_receive into bit_manchester
		bit_buffer = rc5_data;
		bit_man = bit_man + bit_buffer;		
		bit_man = bit_man << 1;
		rc5_data = rc5_data >> 1;
		bit_buffer = rc5_data;
		bit_man = bit_man + bit_buffer;		
		rc5_data = rc5_data >> 1;
		//test if bit is 0 (=10) or 1 (=01) or something else
		buffer = buffer << 1;
		switch (bit_man)
		{
			case 0b00000010:
			{
				bit_buffer = 0;
				break;
			}
			case 0b00000001:
			{
				bit_buffer = 1;
				break;
			}
			default:
			{
				buffer = 0xffff;						// destroy buffer might make error detection easier in next stage, depending on which bit the error occured. 0 is not possible, as it is a valid command
				error = 1;
				n = 13;									// ends reading ( = break "for" loop)
				break;
			}
		}
		buffer = buffer + bit_buffer;	
	}
	//enable receiving
	rc5_ready = 0;
	//store togglebit
	bit_buffer = buffer;
	rc5.toggle = bit_buffer;
	buffer = buffer >> 1;
	//store device id (direction interchanged, last received bit on the right)	
	device = 0;
	for (n = 0; n < 5; n++)
	{
		device = device << 1;
		bit_buffer = buffer;
		device = device + bit_buffer;
		buffer = buffer >> 1;
	}
	rc5.device = device;
	//store command (direction interchanged, last received bit on the right)
	command = 0;
	for (n = 0; n < 6; n++)
	{
		command = command << 1;
		bit_buffer = buffer;
		command = command + bit_buffer;
		buffer = buffer >> 1;
	}
	rc5.command = command;
	return error;
}


/*
- test if the received device id is equal to ours. If not, returns "error" as we are not adressed.
- check toggle bit
  -> if it has changed since the last test, the button on the remote has been released and is pressed again (same or otherone). 
  -> if not, the button is still pressed. Then we ignore 4 sequences (~0,5s) to ensure that the command is only executed once
	 on short press. When kept pressed we accept every 2nd received control sequence (= ~5 control frames / s).We also
	 set FAST which is used to react with higher speed in control functions.
	 Not accepted -> error
returns 1 when error occured
*/

bit test_rc5_control(void)
{
	//var
	char device;
	static bit error, toggle, toggle_last, firstrun;
	static char toggle_count;

	error = 0;
	//test device id
	device = rc5.device;
	if (device != MY_DEVICE)
	{
		error = 1;
	}
	toggle = rc5.toggle;
	//when this function is called for the first time, we don't have a last toggle. As the first received 
	//command must be executed, we copy the current toggle and force processing.
	if (firstrun == 0)
	{
		toggle_last = !toggle;
		firstrun = 1;
	}
	//test if toggle has changed since last test
	if (toggle == toggle_last)
	{
		toggle_count++;
	}
	else
	{
		toggle_count = 0;
		rc5.speed = SLOW;
	}
	//store toggle for next test
	toggle_last = toggle;
	//test if the command is accepted (0, 4, 6) or not.
	switch (toggle_count)
	{
		case 0:
		{
			break;
		}
		case 4:
		{	
			break;
		}	
		case 6:
		{
			toggle_count = 4;
			rc5.speed = FAST;
			break;
		}
		default:
		{
			error = 1;
			break;
		}
	}	
	return error;
}