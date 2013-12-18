#include "common.h"

#ifndef KEYBOARD_H
#define KEYBOARD_H

#define SCAN_CODE_PORT	0x60
#define DEFAULT_CHAR	0x20 // Space

extern char lower_case_map[];
extern char upper_case_map[];

unsigned char get_char(unsigned char scan_code);

#endif
