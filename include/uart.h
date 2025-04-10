#pragma once
#include "main.h"
#include "service.h"

/* IO */

uint8_t UART_Transmit(uint8_t *pData, uint32_t size, int timeout);

/* Command handling */

uint8_t ProcessCommand(void);
