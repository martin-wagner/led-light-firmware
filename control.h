extern void set_mode(void);
extern void set_function(void);
extern void mode_dim(void);
extern void onoff(void);
extern void func_color(void);
extern void func_bright(void);
extern void fucn_program(void);
extern void fuc_colorsonoff(void);
extern void fuc_whiteonoff(void);
extern void func_colorselect(void);
extern void func_fadecolor(void);


// this is used to control the program flow
struct Tcontrol
{
	char mode;
	char function;
	char color_button;
	// EEPROM pointer in here is not a C Pointer !!!
	int eepointer;
	//Stores the factor for brightness (0..100)
	int brightness_factor;
} control;

//Stores the momentary and desigred color value
union Tcolor
{
	unsigned long int colors;								//for comparision tasks (== 0 or  == equal)	
	struct
	{
		char red;
		char green;
		char blue;
		char white;
	};
} color, color_desigred;



// definitions for control structure

// mode -> manual/dim operation
#define MANUAL 10
#define DIM 11

// functions (numbers are randomly selected)
#define FUNC_POWER 10						
#define FUNC_COLOR 11
#define FUNC_COLOR_SET 12
#define FUNC_PROGRAM 13
#define FUNC_COLORSONOFF 14
#define FUNC_COLOR_SELECT 16							//stores which color button has been pressed last
#define FUNC_WHITEONOFF 17
#define FUNC_MEMORY 18
#define FUNC_FADING 19
#define FUNC_BRIGHT 20
#define IDLE 0xff


