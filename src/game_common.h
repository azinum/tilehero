// game_common.h

#ifndef _GAME_COMMON_H
#define _GAME_COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ARR_SIZE(ARR) ((sizeof(ARR)) / (sizeof(ARR[0])))
#define PATH_LENGTH_MAX 512

typedef int32_t i32;
typedef uint32_t u32;
typedef int16_t i16;
typedef uint16_t u16;
typedef int8_t i8;
typedef uint8_t u8;

#endif
