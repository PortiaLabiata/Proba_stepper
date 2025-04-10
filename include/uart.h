#pragma once
#include "main.h"
#include "service.h"

/* IO */

uint8_t UART_Transmit(uint8_t *pData, uint32_t size, int timeout);

uint8_t UART_Recieve(uint8_t *pData, uint32_t size);

/* ISRs */

void USART1_IRQHandler(void);
