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

#ifndef CONTROL_H
#define CONTROL_H

extern void set_mode(void);
extern void set_function(void);
extern void set_brightness(void);
extern void mode_dim(void);
extern void onoff(void);
extern void func_color(void);
extern void func_color_set(void);
extern void fucn_program(void);
extern void fuc_colorsonoff(void);
extern void fuc_whiteonoff(void);
extern void func_colorselect(void);
extern void func_fadecolor(void);
extern void brightness(void);

// this is used to control the program flow
struct Tcontrol
{
	char mode;
	char function;
	char color_button;
	// EEPROM pointer in here is not a C Pointer !!!
	int eepointer;
	char brightness_set;
	char brightness_factor;
	char brightness_old;
	char dim_mode_speed;
	char power_up_mode;
} control;

//Stores the momentary and desigred color value
struct Tcolor
{
	int red;
	int green;
	int blue;
	int white;
} color, color_desigred, color_old;

// definitions for control structure

// mode -> manual/dim operation
#define MANUAL 10
#define DIM 11
#define PROGRAM 12

// functions (numbers are randomly selected)
#define FUNC_POWER 10						
#define FUNC_COLOR 11
#define FUNC_COLOR_SET 12
#define FUNC_PROGRAM 13
#define FUNC_COLORSONOFF 14
#define FUNC_COLOR_SELECT 16							//stores which color button has been pressed last
#define FUNC_WHITEONOFF 17
#define FUNC_FADING 18
#define IDLE 0xff

//brightness_set
#define BRIGHT_SET 10
#define BRIGHT_OK 0

#endif
