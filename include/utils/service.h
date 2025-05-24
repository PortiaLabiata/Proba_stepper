#pragma once
#include "main.h"

/* Macros */

#if SW_LOGGING_ENABLE==1
    #define LOG(__MESSAGE__) ITM_Print(__MESSAGE__, strlen(__MESSAGE__))
#else
    #define LOG(__MESSAGE__) UART_Transmit(ctx.uart_handle, __MESSAGE__, strlen(__MESSAGE__), MAX_TIMEOUT)
#endif

/* Prototypes */

void delay(uint32_t ms);
uint32_t Get_CurrentTick(void);
void ITM_Print(char *output, int length);
