
#include <htc.h>								// Processor definition file
#include "stdlib.h"								// standard library
#include "remote.h"
#include "control.h"
#include "eeprom.h"
#include "program.h"
#include "IO.h"




void init(void);


void main(void)
{
	//variables
	char n, function, test;
	unsigned int temp;
	static bit error;


	//##settings##
	init();
	load_eeprom();
	ei();										// enable interrupts


	//main program loop
	while (1 == 1)
	{
/*
		rc5_data = 0b10010110101001010110010101;
		rc5_ready = 1;
//*/
	//  todo watchdog aktivieren wenn abst"urzproblem behoben und programm fertig// sollte behoben sein, hat sich im interrupt aufgeh"angt
		if (rc5_ready == 1)
		{
			// Gets Data from Remote, check for receive errors
			// todo: st"urzt nach x tastendr"ucken/zeit (?) ab
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
	//  for testing only.
		control.mode = MANUAL;
		control.function = FUNC_COLOR;
		rc5.command = CMD_RED;
//*/

		//Start auto fading mode
		if (control.mode == DIM)
		{
			mode_dim();
			LED_MODE = 1;
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
		LED_MODE = 0;
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
		temp = color.red * control.brightness_factor;
		temp = temp / 255;
		PWM_RED = temp;
		temp = color.green * control.brightness_factor;
		temp = temp / 255;
		PWM_GREEN = temp;
		temp = color.blue * control.brightness_factor;
		temp = temp / 255;
		PWM_BLUE = temp;
		temp = color.white * control.brightness_factor;
		temp = temp / 255;
		PWM_WHITE = temp;


		//check if supply voltage drops out and store control data and colors
		if (SUPPLY == 0)
		{
			write_eeprom();
			//wait until power finally drops out
			while (1 == 1);
		}
	}//while	
}//main


/*
initialization process
*/
void init(void)
{
	//set up oscillator
	OSCCON = 0b11101010;							// internal oscillator 4Mhz

	//set up io ports
	PORTA = 0x00;									// prevent ports from possibility of short circuits when switched to output
	PORTB = 0x00;
	TRISB = 0b00000101;								// 1 = input
	TRISA = 0b00000000;								

	//set up adc (I don't use the adc in here)
	ANSELA = 0x00;									// analog input configuration register must be set
	ANSELB = 0x00;
//	ADCON1 = 0b01110000;							// set prescaler
//	ADCON0 = 0b00000001;							// left adjust, Vref = Vdd, ch0, A/D on
//	ANSELA = 0b00000001;							// select channel (do this in code when changed more than once)
	
	//set up pwm. period length depends on timer2 prescaler and timer reset value => prescaler 64 and reset 128 => 120 Hz.
	T2CON = 0b1101111;								// timer on, prescaler; postscaler for dimming speed (slow)
	T4CON = 0b1110111;								// dimming speed medium
	T6CON = 0b1111111;								// dimming speed fast. slow, medium and fast are only slightly different and support random while dimming
	PR2 = 128;										// timer reset value (pwm period length)
	PR4 = 128;
	PR6 = 128;

	CCPTMRS = 0b00000000;							// select timer2 for all pwm channels
	CCP1CON = 0b00001100;							// single output pwm channel 1
	CCP2CON = 0b00001100;							// single output pwm channel 2
	CCP3CON = 0b00001100;							// single output pwm channel 3
	CCP4CON = 0b00001100;							// single output pwm channel 4

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

	//set up timer 0 (used for delay in remote control protocol\
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
