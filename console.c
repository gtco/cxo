#include "console.h"
#include "library.h"

static volatile unsigned char* g_screen = (unsigned char*) 0xb8000;

static unsigned char height = 25;
static unsigned char width = 80;

static unsigned char xpos;
static unsigned char ypos;

unsigned char BLACK         = 0 ;
unsigned char BLUE          = 1 ;
unsigned char GREEN         = 2 ;
unsigned char CYAN          = 3 ;
unsigned char RED           = 4 ;
unsigned char WHITE         = 7 ;
unsigned char DARK_GREY     = 8 ;
unsigned char BRIGHT_BLUE   = 9 ;
unsigned char BRIGHT_GREEN  = 10;
unsigned char BRIGHT_CYAN   = 11;
unsigned char PINK          = 12;
unsigned char YELLOW        = 14;
unsigned char BRIGHT_WHITE  = 15;

unsigned char TEXT_COLOR    = 7 ;

void clear_screen()
{
    int i, count;
    count = (height * width * 2) - 1;
    for (i = 0; i < count; i++)
    {
	    *(g_screen + i) = 0;
    }
    xpos = ypos = 0;
}

void write_char(int offset, char c, char color)
{
    *(g_screen + offset) = c;
    *(g_screen + offset + 1) = color;
}

void write_string(int offset, char* s, char color)
{
    char* p = s;
	while (*p != '\0')
	{
	    write_char(offset, *p, color);
		offset += 2;
		++p;
	}
}

void new_line()
{
    xpos = 0;
    ++ypos;
}

void put_char(int c)
{
    if (c == '\n')
	{
	    new_line();
	}
	else
	{
        write_char((xpos + ypos * width) * 2, (c & 0xFF), TEXT_COLOR);
		++xpos;
		if (xpos >= width)
		{
		    new_line();
		}
    }
	if(ypos >= height)
	{
		ypos = height - 1;
		scroll();
	}

}

void scroll(void)
{
	unsigned int i = 0, byte_count = 0, offset = 0;

	// Scroll up one line
	byte_count = ( height - 1) * width * 2;

	// Move text
	_memcpy((unsigned long) (g_screen), (unsigned long) (g_screen + width * 2),
		byte_count);

	// Fill the bottom line with blanks
	offset = (height - 1) * width * 2;
	for(i = 0; i < (width*2); i+=2)
	{
		write_char((offset+i), ' ', TEXT_COLOR);
	}
}

void kprintf (const char *format, ...)
{
    char **arg = (char **) &format;
    int c;
    char buf[32];
	_memset(buf, 0, sizeof(buf));

    arg++;

    while ((c = *format++) != 0)
	{
	    if (c != '%')
		    put_char (c);
	    else
	    {
            char *p;
	        c = *format++;
			switch (c)
			{
				case 'd':
					_itoa (buf, (int) *((int *) arg++), c);
					p = buf;
					goto string;
					break;
				case 'u':
				case 'x':
					_ultoa (buf, (unsigned long) *((int *) arg++), c);
					p = buf;
					goto string;
					break;
				case 's':
					p = *arg++;
					if (! p)
					    p = "(empty)";
				string:
					while (*p)
					put_char (*p++);
					break;

				default:
					put_char (*((int *) arg++));
					break;
    		}
		}
	}
}

