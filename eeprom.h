extern void load_eeprom(void);
extern void write_eeprom(void);
extern void read_color(char adr);
extern void write_color(char adr);
extern void write_byte(char adr, char byte);

//******************
//address definitions for EEPROM
//******************
#define EE_RUNDATA 0
#define EE_RED 40
#define EE_YELLOW 44
#define EE_BLUE 48
#define EE_DARKORANGE 52
#define EE_GREEN 56
#define EE_PURPLE 60
#define EE_ORANGE 64
#define EE_TURQUOISE 68
#define EE_PINK 72
#define EE_WARMWHITE 76
#define EE_COOLWHITE 80
#define EE_DAYLIGHT 84
#define EE_PROG0 88
#define EE_PROG1 96
#define EE_PROG2 104
#define EE_PROG3 112
#define EE_PROG4 120
#define EE_PROG5 128
#define EE_PROG6 136
#define EE_PROG7 144
#define EE_PROG8 152
#define EE_PROG9 160
#define EE_COUNT 253
//limits
#define EE_MINPROG 88
#define EE_MAXPROG 160
