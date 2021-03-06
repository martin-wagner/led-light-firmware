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
#include "IO.h"
#include "sevenseg.h"

//Bits 7seg display
union T7seg
{
	char segments;
	struct
	{	  
		char sega:1;		   
		char segb:1;
		char segc:1;
		char segd:1;
		char sege:1;
		char segf:1;
		char segg:1;
		char dp:1;   //not used
	};		
};

const char seg_table[26] = 
{
	//0gfedcba
	0b00111111,	//0
	0b00000110,	//1
	0b01011011,	//2
	0b01001111, //3
	0b01100110,	//4
	0b01101101,	//5
	0b01111101, //6
	0b00000111,	//7
	0b01111111,	//8
	0b01101111,	//9
	0b01110111,	//A
	0b01111100,	//b
	0b00111001, //C
	0b01011110,	//d
	0b01111001,	//E
	0b01110001,	//F
	0b00111101, //G
	0b01110110,	//H
	0b00000100,	//i
	0b00001110,	//J
	0b00111000,	//L
	0b00110111,	//n
	0b01110011, //P
	0b00111110,	//U
	0b01101110,	//Y
	0b00000000	//NIL
};


/*
lookup table for 7seg display
gets: character to show (0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F,G,H,J,L,n,P,U,Y)
letters must be S + capital letters (SA, SB..), numbers just numbers
S0 switches digits off
*/
void lookup(char character)
{
	union T7seg sseg;
	//select character
	sseg.segments = seg_table[character];
	//set bits
	SEGA = sseg.sega;
	SEGB = sseg.segb;
	SEGC = sseg.segc;
	SEGD = sseg.segd;
	SEGE = sseg.sege;
	SEGF = sseg.segf;
	SEGG = sseg.segg;
}
