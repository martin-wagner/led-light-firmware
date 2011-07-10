
#include <htc.h>								// Processor definition file
#include "stdlib.h"								// standard library
#include "remote.h"
#include "control.h"
#include "eeprom.h"
#include "program.h"
#include "sevenseg.h"
#include "IO.h"

__CONFIG(0x39dc);
__CONFIG(0x3fff);


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
		if (control.mode == MANUAL)
		{
			function = control.function;
			switch (function)
			{
				case FUNC_POWER:
				{
					onoff();
					break;
				}
				case FUNC_COLOR:
				{
					func_color();
					break;
				}
				case FUNC_COLOR_SET:
				{
					func_color_set();
					break;
				}
				case FUNC_PROGRAM:
				{
					fucn_program();
					break;
				}
				case FUNC_COLORSONOFF:
				{
					fuc_colorsonoff();
					break;
				}
				case FUNC_COLOR_SELECT:
				{	
					func_colorselect();
					break;
				}
				case FUNC_WHITEONOFF:
				{	
					fuc_whiteonoff();
					break;
				}
				case FUNC_FADING:
				{	
					func_fadecolor();
					break;
				}
				//idle state
				default:
				{																																																																																																																																																																																																																																																																																																																																																																																																																																																																																
					//do nothing
				}
			}//switch		
		}//if program is selected. program mode can only be exited by switching off power or going trough
		if (control.mode == PROGRAM)
		{
			program();
		}
		//chech if brightness up/down is requested, increase or decrease global brightness
		if (control.brightness_set == BRIGHT_SET)
		{
			//increase/decrease overall brighness
			brightness();	
			//reset request
			control.brightness_set = BRIGHT_OK;
		}
		//calculate PWM values including brightness and update PWM values
		temp = color.red;
		temp = (control.brightness_factor * temp) >> 8;		// >>8 = /0xff
		//high
		PWM_RED = temp >> 2;
		//two low bits
		PWM_RED_L = temp;
		temp = color.green;
		temp = (control.brightness_factor * temp) >> 8;
		//high
		PWM_GREEN = temp >> 2;
		//two low bits
		PWM_GREEN_L = temp;
		temp = color.blue;// * control.brightness_factor;
		temp = (control.brightness_factor * temp) >> 8;
		//high
		PWM_BLUE = temp >> 2;
		//two low bits
		PWM_BLUE_L = temp;
		temp = color.white;// * control.brightness_factor;
		temp = (control.brightness_factor * temp) >> 8;
		//high
		PWM_WHITE = temp >> 2;
		//two low bits
		PWM_WHITE_L = temp;
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
initialization process
*/
void init(void)
{
	//set up oscillator for 32 MHz operation. Program flow doesn't need this speed, but otherwise 20 kHz PWM frequency is not possible
	/*
     // ----------------------------------------------------------
     // Configure oscillator for 32MHz operation
     // ----------------------------------------------------------
     // Bit    <7>       : PLL enabled via config word i.e. <0>
     // Bits  <6-3>    : 32MHz operation i.e. <1110>
     // Bit    <2>       : Unimplemented i.e. <0>
     // Bit    <1:0>    : Let config word select oscillator i.e. <00>
     // ----------------------------------------------------------
	*/
    OSCCON = 0x70;
	
	//set up pwm. period length depends on timer2 prescaler and timer reset value => prescaler 00 and reset 255 => 32 kHz.
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
	T2CON = 0b1101100;								// timer on, prescaler; postscaler for dimming speed (slow)
	T4CON = 0b1110100;								// dimming speed medium
	T6CON = 0b1111100;								// dimming speed fast. slow, medium and fast are only slightly different and support random while dimming
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
	ADCS = 110 F ADC = Fosc/64
	bit 4 = 0
	ADNREF = 0 neg ref = Vss
	ADPREF = 00 Vref = Vdd
	*/
	ADCON1 = 0b11100000;

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
	//PS 001 = Prescaler 64
	PS0 = 1;
	PS1 = 0;
	PS2 = 1;

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
