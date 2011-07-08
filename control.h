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
} color, color_desigred;



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
