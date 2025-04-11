#pragma once
#include "main.h"
#include "service.h"

/* IO */

uint8_t UART_Transmit(UART_Handle_t *handle, uint8_t *pData, uint32_t size, int timeout);
uint8_t UART_Recieve(UART_Handle_t *handle, uint8_t *pData, uint32_t size);

void UART_RecieveCallback(UART_Handle_t *handle);

