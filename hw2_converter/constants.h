#pragma once
#include <stdint.h>


typedef enum {
	Help,
	Convert,
	InvalidArgs
} ArgsSet;

typedef enum {
	CP1251,
	KOI8,
	ISO88595,
	InvalidCodec
} Codec;

typedef struct {
	uint8_t bytes;
	uint8_t ch[4];
} utf8_char;
