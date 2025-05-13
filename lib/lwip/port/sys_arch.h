#pragma once
#include <stm32f103x6.h>

#define BYTE_ORDER   LITTLE_ENDIAN

void ITM_Print(char *str, int size);