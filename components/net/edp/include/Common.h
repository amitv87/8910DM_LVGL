#ifndef __COMMON_H__
#define __COMMON_H__

#include "unistd.h"
#include "stdbool.h"
#include "stdint.h"
/*---------------------------------------------------------------------------*/
/* Type Definition Macros                                                    */
/*---------------------------------------------------------------------------*/
#ifndef __WORDSIZE
/* Assume 32 */
#define __WORDSIZE 32
#endif

typedef uint8_t uint8;
typedef int8_t int8;

typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef bool boolean;

#if defined(_LINUX) || defined(WIN32)
typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;
#endif

#ifdef WIN32
typedef int socklen_t;
#endif

#if defined(WIN32)
typedef unsigned long long int uint64;
typedef long long int int64;
#elif (__WORDSIZE == 32)
__extension__ typedef long long int int64;
__extension__ typedef unsigned long long int uint64;
#elif (__WORDSIZE == 64)
typedef unsigned long int uint64;
typedef long int int64;
#endif

void edp_printf(const char *format, ...) __attribute__((format(printf, 1, 2)));

#define printf(...) edp_printf(__VA_ARGS__)

#endif /* __COMMON_H__ */
