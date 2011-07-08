extern bit get_rc5_control(void);
extern bit test_rc5_control(void);

volatile long unsigned int rc5_data;
volatile char rc5_ready;


struct Trc5
{
	char toggle;
	char device;
	char command;
	char speed;
} rc5;

//definition to speed 
#define SLOW 10
#define FAST 20

/*
#####################################
definitions for the remote control
last received bit is on the right
#####################################
*/
/*
device ID - 5 bits - is equal on all commands
				   Bit 0   5
					   |   |
					   V   V
*/						
#define MY_DEVICE 0b00001110

/*
command - 6 bits 
							Bit 0    6
								|	 |
								|    |
							    V    V
*/	
//Power
#define CMD_POWER 			0b00001100
//colors on/off
#define CMD_COLORSONOFF 	0b00010000
//white on/off
#define CMD_WHITEONOFF	 	0b00001010
//red
#define CMD_RED 			0b00001000
//green
#define CMD_GREEN 			0b00000010
//dark blue
#define CMD_DARKBLUE 		0b00000011
//dark orange
#define CMD_DARKORANGE 		0b00000100
//dark green
#define CMD_DARKGREEN 		0b00000101
//purple
#define CMD_PURPLE 			0b00000110
//orange
#define CMD_ORANGE			0b00000111
//turquoise
#define CMD_TURQUOISE 		0b00000001
//pink
#define CMD_PINK 			0b00001001
//yellow
#define CMD_YELLOW 			0b00001110
//blue
#define CMD_BLUE 			0b00000000
//brown
#define CMD_BROWN			0b00101100
//bright -
#define CMD_BRIGHTDEC 		0b00110011
//bright +
#define CMD_BRIGHTINC 		0b00110010
//program -
#define CMD_PROGRAMDEC 		0b00110101
//program +
#define CMD_PROGRAMINC 		0b00110100
//select red button
#define CMD_REDBUTTON 		0b00001101
//select green button
#define CMD_GREENBUTTON 	0b00101010
//select white button (As I don't have a white one, I use the yellow one instead)
#define CMD_WHITEBUTTON 	0b00010101
//select blue button
#define CMD_BLUEBUTTON		0b00101011
//up
#define CMD_UP 				0b00010011
//left
#define CMD_LEFT			0b00010110
//OK
#define CMD_OK 				0b00010001
//right
#define CMD_RIGHT 			0b00101111
//down
#define CMD_DOWN 			0b00101110
//memory programming function
#define CMD_PROGRAM			0b00100000
//cancel
#define CMD_CANCEL 			0b00101101
//command musn't be 0xff, this is used as 'invalid command'!!!!!!!