#pragma once
#include "core/system.h"
#include "core/types.h"
#include "utils/service.h"

#define MAX_UARTS   3UL

/* Typedefs */

typedef struct UART_Handle UART_Handle_t;

/* Configuration */

UART_Handle_t *UART_Init(USART_TypeDef *inst);

/* IO */

UART_Status_t UART_Transmit(UART_Handle_t *handle, uint8_t *pData, uint32_t size, int timeout);
UART_Status_t UART_Recieve(UART_Handle_t *handle, volatile uint8_t *pData, uint32_t size);

/* Getters/setters */

uint8_t UART_GetCmdRdy(UART_Handle_t *handle);
uint8_t UART_GetRxLeft(UART_Handle_t *handle);

void UART_SetCmdRdy(UART_Handle_t *handle, uint8_t status);
void UART_SetRxLeft(UART_Handle_t *handle, uint8_t value);

uint8_t UART_RecieveCallback(System_Context_t *ctx);

