#ifndef TYPES_H
#define TYPES_H

// from stdint.h

#ifndef __int8_t_defined
#define __int8_t_defined
typedef signed char int8_t;
typedef short int16_t;
typedef long int32_t;
typedef long long int64_t;
#endif

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
#ifndef __uint32_t_defined
#define __uint32_t_defined
typedef unsigned long uint32_t;
#endif
typedef unsigned long long uint64_t;

/* Minimum-width integer types */

typedef signed char int_least8_t;
typedef short int_least16_t;
typedef long int_least32_t;
typedef long long int_least64_t;

typedef unsigned char uint_least8_t;
typedef unsigned short uint_least16_t;
typedef unsigned long uint_least32_t;
typedef unsigned long long uint_least64_t;

/* Fastest minimum-width integer types */

typedef signed char int_fast8_t;
typedef long int_fast16_t;
typedef long int_fast32_t;
typedef long long int_fast64_t;

typedef unsigned char uint_fast8_t;
typedef unsigned long uint_fast16_t;
typedef unsigned long uint_fast32_t;
typedef unsigned long long uint_fast64_t;

/* Integer types capable of holding object pointers */

#ifndef __intptr_t_defined
#define __intptr_t_defined
typedef long intptr_t;
#endif
typedef unsigned long uintptr_t;

#endif