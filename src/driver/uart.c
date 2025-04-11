#include "driver/uart.h"

/* Typedefs */

struct UART_Handle {
    volatile USART_TypeDef *instance;
    volatile uint8_t *cursor;
    volatile uint8_t error_code;
    volatile uint8_t command_ready;
    volatile uint32_t rx_left;
};

/* Handle pool */

static UART_Handle_t _uart_pools[MAX_UARTS];
static uint8_t _n_uarts = 0;

/* Configuration */

UART_Handle_t *UART_Init(USART_TypeDef *inst) {
    UART_Handle_t *handle = &_uart_pools[_n_uarts++];
    handle->rx_left = 0;
    handle->command_ready = RESET;
    handle->instance = inst;
    return handle;
}

/* IO */

uint8_t UART_Transmit(UART_Handle_t *handle, uint8_t *pData, uint32_t size, int timeout) {
    uint32_t ms_start = Get_CurrentTick();

    for (uint8_t *data = pData; data - pData < size; data++) {
        while (!(handle->instance->SR & USART_SR_TXE_Msk)) {
            if (timeout != MAX_TIMEOUT && Get_CurrentTick() - ms_start > timeout) {
                return RESET;
            }
        } // Wait until TXE bit is set.
        handle->instance->DR = *data; // Transmit
    }
    while (!(handle->instance->SR & USART_SR_TC_Msk)) {} // Wait until TC flag is set
    return SET;
}

uint8_t UART_Recieve(UART_Handle_t *handle, uint8_t *pData, uint32_t size) {
    handle->rx_left = size;
    handle->cursor = pData;
    return SET;
}

/* Getters/setters */

uint8_t UART_GetCmdRdy(UART_Handle_t *handle) {
    return handle->command_ready;
}

uint8_t UART_GetRxLeft(UART_Handle_t *handle) {
    return handle->rx_left;
}

void UART_SetCmdRdy(UART_Handle_t *handle, uint8_t status) {
    handle->command_ready = status;
}

void UART_SetRxLeft(UART_Handle_t *handle, uint8_t value) {
    handle->rx_left = value;
}

/* Callbacks */

uint8_t UART_RecieveCallback(System_Context_t *ctx) {
    UART_Handle_t *handle = ctx->uart_handle;
    if (handle->instance->SR & USART_SR_RXNE_Msk) { // Character recieved
        if (handle->rx_left == 1) {
            handle->command_ready = SET;
        }
        *handle->cursor = handle->instance->DR;
        handle->cursor++;
        handle->rx_left--;
    }
   return SET;
}
