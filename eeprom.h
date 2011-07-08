extern void load_eeprom(void);
extern void write_eeprom(void);
extern void read_color(char adr);
extern void write_color(char adr);

//******************
//address definitions for EEPROM
//******************
#define EE_RUNDATA 0
#define EE_RED 40
#define EE_GREEN 44
#define EE_DARKBLUE 48
#define EE_DARKORANGE 52
#define EE_DARKGREEN 56
#define EE_PURPLE 60
#define EE_ORANGE 64
#define EE_TURQUOISE 68
#define EE_PINK 72
#define EE_YELLOW 76
#define EE_BLUE 80
#define EE_BROWN 84
#define EE_PROG0 88
#define EE_PROG1 92
#define EE_PROG2 96
#define EE_PROG3 100
#define EE_PROG4 104
#define EE_PROG5 108
#define EE_PROG6 112
#define EE_PROG7 116
#define EE_PROG8 120
#define EE_PROG9 124
#define EE_COUNT 254
//limits
#define EE_MINPROG 88
#define EE_MAXPROG 124
