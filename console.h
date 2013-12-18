#ifndef CONSOLE_H
#define CONSOLE_H

//  circular queue
typedef struct queue_s
{
	unsigned char *data;
	unsigned long size;
	unsigned long in_base;
	unsigned long in_ptr;
	unsigned long out_base;
	unsigned long out_ptr;
} queue_t;

typedef struct console_s
{
/* virtual console input */
	queue_t keystrokes;
/* virtual console output */
	unsigned long esc, attrib, csr_x, csr_y, esc1, esc2, esc3;
	unsigned short *fb_adr;
} console_t;

extern unsigned char BLACK;
extern unsigned char BLUE;
extern unsigned char GREEN;
extern unsigned char CYAN;
extern unsigned char RED;
extern unsigned char WHITE;
extern unsigned char DARK_GREY;
extern unsigned char BRIGHT_BLUE;
extern unsigned char BRIGHT_GREEN;
extern unsigned char BRIGHT_CYAN;
extern unsigned char PINK;
extern unsigned char YELLOW;
extern unsigned char BRIGHT_WHITE;
extern unsigned char TEXT_COLOR;

void clear_screen();
void write_char(int offset, char c, char color);
void write_string(int offset, char* s, char color);
void kprintf (const char *format, ...);
void put_char(int c);
void new_line();
void scroll(void);

#endif
