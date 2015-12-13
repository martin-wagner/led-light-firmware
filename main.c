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
#include <stdlib.h>								// standard library
#include "remote.h"
#include "control.h"
#include "eeprom.h"
#include "program.h"
#include "sevenseg.h"
#include "IO.h"

__CONFIG(0x39dc);
__CONFIG(0x3fff);

int brightness_correction(int linear);
void init(void);

void main(void)
{
	char n, function, test = 0;
	unsigned int supply;
	short long int temp;
	static bit error;

	init();
	load_eeprom();
	ADGO = 1;									// start first a/d conversation
	ei();										// enable interrupts
	//main program loop
	while (1 == 1)
	{
		/*
		rc5_data = 0b10010110101001010110010101;
		rc5_ready = 1;
		//*/

		//watchdog reset
		CLRWDT();

		if (rc5_ready == 1)
		{
			// Gets Data from Remote, check for receive errors
			error = get_rc5_control();
			error = error || test_rc5_control();
			// if receive worked correct, check which mode and which function is selected (dim or manual)
			if (error == 0)
			{
				set_mode();
				set_function();
				set_brightness();
			}
			else
			{
				rc5.command = 0xff;
			}
		}//if

		/*
		//  for debugging only.
		control.mode = MANUAL;
		control.function = FUNC_COLOR;
		rc5.command = CMD_RED;
		//*/

		//Start auto fading mode
		if (control.mode == DIM)
		{
			mode_dim();
		}
		//manual mode
		else if (control.mode == MANUAL)
		{
			function = control.function;
			switch (function)
			{
				case FUNC_POWER:
					onoff();
					break;
				case FUNC_COLOR:
					func_color();
					break;
				case FUNC_COLOR_SET:
					func_color_set();
					break;
				case FUNC_PROGRAM:
					fucn_program();
					break;
				case FUNC_COLORSONOFF:
					fuc_colorsonoff();
					break;
				case FUNC_COLOR_SELECT:	
					func_colorselect();
					break;
				case FUNC_WHITEONOFF:	
					fuc_whiteonoff();
					break;
				case FUNC_FADING:	
					func_fadecolor();
					break;
				//idle state
				default:	
					break;																																																																																																																																																																																																																																																																																																																																																																																																																																																	
			}//switch		
		}//if program is selected. program mode can only be exited by switching off power or going trough
		else if (control.mode == PROGRAM)
		{
			program();
		}
		//check if brightness up/down is requested, increase or decrease global brightness
		if (control.brightness_set == BRIGHT_SET)
		{
			//increase/decrease overall brighness
			brightness();	
			//reset request
			control.brightness_set = BRIGHT_OK;
		}
		//calculate PWM values including brightness and update PWM values if necessary
		if ((color.red != color_old.red) ||
			(control.brightness_old != control.brightness_factor))
		{
			temp = color.red;
			temp = (control.brightness_factor * temp) >> 8;		// >>8 = /0xff
			temp = brightness_correction(temp);
			//high
			PWM_RED = temp >> 2;
			//two low bits
			PWM_RED_L = temp;
		}
		if ((color.green != color_old.green) ||
			(control.brightness_old != control.brightness_factor))
		{
			temp = color.green;
			temp = (control.brightness_factor * temp) >> 8;
			temp = brightness_correction(temp);
			//high
			PWM_GREEN = temp >> 2;
			//two low bits
			PWM_GREEN_L = temp;
		}
		if ((color.blue != color_old.blue) ||
			(control.brightness_old != control.brightness_factor))
		{
			temp = color.blue;
			temp = (control.brightness_factor * temp) >> 8;
			temp = brightness_correction(temp);
			//high
			PWM_BLUE = temp >> 2;
			//two low bits
			PWM_BLUE_L = temp;
		}
		if ((color.white != color_old.white) ||
			(control.brightness_old != control.brightness_factor))
		{
			temp = color.white;
			temp = (control.brightness_factor * temp) >> 8;
			temp = brightness_correction(temp);
			//high
			PWM_WHITE = temp >> 2;
			//two low bits
			PWM_WHITE_L = temp;
		}
		color_old = color;
		control.brightness_old = control.brightness_factor;
		//check if supply voltage drops out and store control data and colors
		if (ADGO == 0)		//-> a/d conversation finished
		{
			supply = ADRES;
			//start new conversation
			ADGO = 1;
			//if supply < ~4.8V
			if (supply < 1000)
			{
				//when power finally drops out, the pic restarts several times (and then comes here straight again). 
				//this prevents that the eeprom ist overwritten in this stage.
				//todo: how do I assure that the adc doesn't do any strange things in this stage (like produce values between 800 and 1024)
				if ((supply > 800) && (supply < 1024))
				{
					write_eeprom();
				}
				//switch on display to drain capacitor
				lookup(8);
				LED_STATUS = 1;
				//here we wait until either power drops out finally or power is switched back on. then we do a watchdot restart
				while (1 == 1)
				{
					ADGO = 1;
					while (ADGO == 1){};
					supply = ADRES;
					if (supply != 1023)
					{
						//watchdog reset
						CLRWDT();
					}
				}
			}
		}
	}//while	
}//main

/*
Brightness Correction
this is done according to http://neuroelec.com/2011/04/led-brightness-to-your-eye-gamma-correction-no/
and "“Gamma” and its Disguises: The Nonlinear Mappings of Intensity in Perception, CRTs, Film and Video
	 By Charles A. Poynton"

based on CIE L* function "L* = 116(Y / Yn)^1/3 -16" which describes the perception of the human eye
re-arranged to get necessary intensity of LEDs as an integer value from 0..1023
-> pwm = ((16 * 10.23 + linear) / 116 * 10.23)^3 * 100 * 10.23
optimized to use float multiplication as only float operation
-> pwm = ((164 + linear) * 0.00084268716)^3 * 1023
*/
int brightness_correction(int linear)
{
	float result;
	int normalized;
		
	// the normalization formula doesn't work with low values, so we have to use
	// linear values instead
	if (linear == 0)
	{
		return 0;
	}
	else if (linear < 6) 
	{
		return 1;
	}
	else if (linear < 11) 
	{
		return 2;
	}
	else if (linear < 15) 
	{
		return 3;
	}
	else 
	{
		result = ((164 + linear) * 0.00084268716);
		result = result * result * result * 1023;
		normalized = (int)result + 1;
	}
	// is normalized value plausible
	if (normalized > 1023)
	{
		normalized = 1023;
	}
	if (normalized < 3)
	{
		normalized = 3;
	}
	return normalized;
}

/*
initialization process
*/
void init(void)
{
	/*
	Alignment of Bits: bit 7 ... bit 0
	*/
	
	//set up oscillator for 4 MHz operation.
	OSCCON = 0b11101010;						
	
	/* 
	set up pwm. period length depends on timer2 prescaler and timer reset value => prescaler 16 and reset 255 => 250 Hz. 
	*/
	//switch all io to input
	TRISA = 0xff;
	TRISB = 0xff;	
	//switch off outputs
	PWM_RED = 0;
	PWM_GREEN = 0;
	PWM_BLUE = 0;
	PWM_WHITE = 0;
	/*
	TxCON
	bit 7 = 0
	TOUTPS (bit 6..3) postscaler; 1111 = 16, 1110 = 15 and so on
	TMRxON = 1
	TxCKPS = 00 prescaler off
	*/
	T2CON = 0b0101110;								// timer on, prescaler; postscaler for dimming speed (slow)
	T4CON = 0b0110110;								// dimming speed medium
	T6CON = 0b0111110;								// dimming speed fast. slow, medium and fast are only slightly different and support random while dimming
	PR2 = 0xff;										// timer reset value (pwm period length, for max. resolution must be 0xff)
	PR4 = 0xff;
	PR6 = 0xff;

	PWM_RED = 0;
	PWM_GREEN = 0;
	PWM_BLUE = 0;
	PWM_WHITE = 0;
	CCPTMRS = 0b00000000;							// select timer2 for all pwm channels
	CCP1CON = 0b00001100;							// single output pwm channel 1
	CCP2CON = 0b00001100;							// single output pwm channel 2
	CCP3CON = 0b00001100;							// single output pwm channel 3
	CCP4CON = 0b00001100;							// single output pwm channel 4

	//set up io ports
	PORTA = 0x00;									// prevent ports from possibility of short circuits when switched to output
	PORTB = 0x00;
	TRISB = 0b00000101;								// 1 = input
	TRISA = 0b00000000;								

	//set up adc (used to detect voltage dropout)
	ANSELA = 0x00;									// analog input configuration register must be configured
	ANSELB = 0b00000100;							// select GPIO used as analogue input	
	/*
	ADCON0
	bit 0 = 0
	CHS = 01010 AN10
	GO/DONE = 0 nothing to do
	ADON = 1 AD is on
	*/
	ADCON0 = 0b00101001;	
	/*
	ADCON1
	ADFM = 1 right adjust result
	ADCS = 101 F ADC = Fosc/16
	bit 4 = 0
	ADNREF = 0 neg ref = Vss
	ADPREF = 00 Vref = Vdd
	*/
	ADCON1 = 0b11010000;

	//set up timer 1 (used for color fading between different fuctions)
	/*
	T1CON
	TMR1CS = 00 f = finstr.
	T1CKPS = 11 prescaler 1 : 8
	T1OSCEN = 0 enables/disables oscillator while in sleep
	!T1SYNC = 0 ignored when internal oscillator is used
	bit 1 = 0
	TMR1ON = 0 timer off
	*/
	T1CON = 0b00110000;
	/*
	TMR1GE = 0 disable timer1 gate functions
	*/
	TMR1GE = 0;

	//set up timer 0 (used for delay in remote control protocol)
	//TMR0CS 0 = Internal instruction cycle clock (FOSC/4)
	TMR0CS = 0;
	//PSA 0 = Prescaler is assigned to the Timer0 module
	PSA = 0;
	//PS 001 = Prescaler 4
	PS0 = 1;
	PS1 = 0;
	PS2 = 0;

	/*set up interrupts
	GIE 0 = Disables all interrupts
	PEIE 0 = Disables all peripheral interrupts	
	TMR0IE 0 = Disables the Timer0 interrupt
	INTE 1 = Enables the INT external interrupt
	IOCIE 0 = Disables the interrupt-on-change
	TMR0IF read
	INTF read
	IOCIF read
	*/
	INTCON = 0b00010000;
	/*
	INTEDG 0 = INT Port interrupt occurs on falling edge !!!MUST BE DONE AFTER SET UP OF TIMER0 AS IT IS ALSO IN OPTION_REG!!!
	*/
	INTEDG = 0;

}
