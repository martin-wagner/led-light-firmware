
#include <htc.h>								// Processor definition file
#include <stdlib.h>								// standard library





void main(void)
{
//variables
unsigned char ad, n1, bright1 = 30, bright2 = 100, bright3 = 180, updown1 = 0, updown2 = 1, updown3 = 0, wait1 = 0x3, wait2 = 0x50, wait3 = 0x100;
unsigned char random;
//##settings##
OSCCON = 0b11101010;							// internal oscillator 4Mhz
TRISB = 0x00;
TRISA = 0b11110111;
//adc
ADCON1 = 0b01110000;							// set prescaler
ADCON0 = 0b00000001;							// left adjust, Vref = Vdd, ch0, A/D on
ANSELA = 0b00000001;	
//tmr2
T2CON = 0b1101110;								// timer on, prescaler; postscaler for dimming speed (slow)
T4CON = 0b1110110;								// dimming speed medium
T6CON = 0b1111110;								// dimming speed fast
PR2 = 0xff;										// timer reset value (pwm period length)
						
//pwm
CCPTMRS = 0b00000000;							// select timer2 for all pwm channels
CCP1CON = 0b00001100;							// single output pwm channel 1
CCP2CON = 0b00001100;							// single output pwm channel 2
CCP3CON = 0b00001100;							// single output pwm channel 3

//main program loop

while (1 == 1)
{
	ADGO = 1;									// start A/D conversation
	while (ADGO == 1){};						// wait until finished
	ad = ADRESL;

	//brightness LED 1
	if (TMR2IF == 1)							// if timer2 postscaler flag is set
	{							
		TMR2IF = 0;								// clear it and
		switch (updown1)						// test if brightness should be increased =0 or decreased =1 or should remain constant >1
		{	
			case (0) :					
			{
				bright1 ++;						// increase brightness by one
				if (bright1 == 0xff)			// when maximum brightness is reached, set flag to start waiting time
				{
						updown1 = 2;
				}
				break;
			}
		
			case (1) :
			{
				bright1 --;						// decrease brightness by one
				if (bright1 == 0)				// when minimum brightness is reached, clear flag to start waiting time
				{
					updown1 = 3;
				}
				break;
			}

			case (2) :
			{
				wait1 --;						// block until wait == 0
				if (wait1 == 0)					// returns to dimming operation and resets waiting timer
				{
					updown1 = 1;
					wait1 = rand();				// creates random number
				}
				break;
			}
			case (3) :
			{
				wait1 --;						// block until wait == 0
				if (wait1 == 0)					// returns to dimming operation and resets waiting timer
				{
					updown1 = 0;
					wait1 = rand();				// creates random number
				}
				break;
			}
		}
		CCPR1L = bright1;						// set PWM puls width with desigred brightness value	
	}
	//brightness LED 2
	if (TMR4IF == 1)
	{											// if timer4 postscaler flag is set
		TMR4IF = 0;								// clear it and
		switch (updown2)						// test if brightness should be increased =0 or decreased =1 or should remain constant >1
		{	
			case (0) :					
			{
				bright2 ++;						// increase brightness by one
				if (bright2 == 0xff)			// when maximum brightness is reached, set flag to start waiting time
				{
						updown2 = 2;
				}
				break;
			}
		
			case (1) :
			{
				bright2 --;						// decrease brightness by one
				if (bright2 == 0)				// when minimum brightness is reached, clear flag to start waiting time
				{
					updown2 = 3;
				}
				break;
			}

			case (2) :
			{
				wait2 --;						// block until wait == 0
				if (wait2 == 0)					// returns to dimming operation and resets waiting timer
				{
					updown2 = 1;
					wait2 = rand();				// creates random number
				}
				break;
			}
			case (3) :
			{
				wait2 --;						// block until wait == 0
				if (wait2 == 0)					// returns to dimming operation and resets waiting timer
				{
					updown2 = 0;
					wait2 = rand();				// creates random number
				}
				break;
			}
		}
		CCPR2L = bright2;						// set PWM puls width with desigred brightness value
	}
	//brightness LED3
	if (TMR6IF == 1)							// if timer6 postscaler flag is set
	{							
		TMR6IF = 0;								// clear it and
		switch (updown3)						// test if brightness should be increased =0 or decreased =1 or should remain constant >1
		{	
			case (0) :					
			{
				bright3 ++;						// increase brightness by one
				if (bright3 == 0xff)			// when maximum brightness is reached, set flag to start waiting time
				{
						updown3 = 2;
				}
				break;
			}
		
			case (1) :
			{
				bright3 --;						// decrease brightness by one
				if (bright3 == 0)				// when minimum brightness is reached, clear flag to start waiting time
				{
					updown3 = 3;
				}
				break;
			}

			case (2) :
			{
				wait3 --;						// block until wait == 0
				if (wait3 == 0)					// returns to dimming operation and resets waiting timer
				{
					updown3 = 1;
					wait3 = rand();				// creates random number
				}
				break;
			}
			case (3) :
			{
				wait3 --;						// block until wait == 0
				if (wait3 == 0)					// returns to dimming operation and resets waiting timer
				{
					updown3 = 0;
					wait3 = rand();				// creates random number
				}
				break;
			}
		}
		CCPR3L = bright3;						// set PWM puls width with desigred brightness value
	}
			

}	
}



