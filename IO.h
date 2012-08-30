#ifndef IO_H
#define IO_H

//PORTA GPIOs
#define SEGD LATA0
#define SEGC LATA1 
#define LED_STATUS LATA2
//CCP3
//CCP4
//MCLR
#define SEGF LATA6
#define SEGE LATA7

//PORTB GPIOs
#define IR_IN RB0
#define LED_RECEIVE LATB1
//adc10					//Input Port for supply voltage test
//CCP1
#define SEGA LATB4
#define SEGB LATB5
//CCP2
#define SEGG LATB7


// Name PWM Outputs
#define PWM_WHITE CCPR1L
#define PWM_WHITE_L CCP1CONbits.DC1B
#define PWM_RED CCPR2L
#define PWM_RED_L CCP2CONbits.DC2B
#define PWM_GREEN CCPR3L
#define PWM_GREEN_L CCP3CONbits.DC3B
#define PWM_BLUE CCPR4L
#define PWM_BLUE_L CCP4CONbits.DC4B

#endif