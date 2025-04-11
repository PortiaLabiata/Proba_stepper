#pragma once
#include "main.h"

/* Typedefs */



/* IO */

uint8_t UART_Transmit(UART_Handle_t *handle, uint8_t *pData, uint32_t size, int timeout);
uint8_t UART_Recieve(UART_Handle_t *handle, uint8_t *pData, uint32_t size);

uint8_t UART_RecieveCallback(USART_TypeDef *usart);

