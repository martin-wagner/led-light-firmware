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
