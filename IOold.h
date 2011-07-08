//PORTA GPIOs
#define SEGB LATA0
#define SEGC LATA1 
#define LED_STATUS LATA2
//CCP3
//CCP4
//MCLR
#define SEGE LATA6
#define SEGA LATA7

//PORTB GPIOs
#define IR_IN RB0
#define LED_RECEIVE LATB1
#define SUPPLY RB2					//Input Port for supply voltage test
//CCP1
#define SEGG LATB4
#define SEGD LATB5
//CCP2
#define SEGF LATB7

// Name PWM Outputs. Low two bits are in config register
#define PWM_GREEN CCPR1L
#define PWM_GREEN_L CCP1CONbits.DC1B
#define PWM_RED CCPR2L
#define PWM_RED_L CCP2CONbits.DC2B
#define PWM_WHITE CCPR3L
#define PWM_WHITE_L CCP3CONbits.DC3B
#define PWM_BLUE CCPR4L
#define PWM_BLUE_L CCP4CONbits.DC4B
