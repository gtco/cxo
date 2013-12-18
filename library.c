#include "library.h"

void _reverse(char s[])
{
    int c = 0, i = 0, j = 0;

    j = _strlen(s) - 1;

    for (i = 0; i < j; i++, j-- )
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void _itoa(char s[], int n, char base)
{
    int i = 0, sign = 0;
    int r = 0, d = 10;

    if (base == 'x')
        d = 16;

	if (n < 0)
	{
		sign = 1;
		n = n * (-1);
	}

    do
    {
        r = n % d;
        s[i++] = (r < 10) ? r + '0' : r + 'A' - 10;
    }
    while ((n /= d) > 0);

    if (sign)
		s[i++] = '-';

    s[i] = '\0';
    _reverse(s);
}

void _ultoa(char s[], unsigned long n, char base)
{
    int i = 0;
    int r = 0, d = 10;

    if (base == 'x')
        d = 16;
    do
    {
        r = n % d;
        s[i++] = (r < 10) ? r + '0' : r + 'A' - 10;
    }
    while ((n /= d) > 0);
    s[i] = '\0';
    _reverse(s);
}


void _memset(char s[], char c, unsigned long n)
{
    int i = 0;
    for (i = 0; i < n; i++)
    {
        s[i] = c;
    }
}

int _strlen(char s[])
{
    int i = 0;
    while (s[i] != '\0')
        ++i;
    return i;
}

unsigned long _memcpy(unsigned long dest, unsigned long src, unsigned int size)
{
    const unsigned char *s = (unsigned char *) src;
    const unsigned char *e = s + size;
    unsigned char *d = (unsigned char *) dest;

    while (s != e)
    {
        *d++ = *s++;
    }

    return (dest);
}
