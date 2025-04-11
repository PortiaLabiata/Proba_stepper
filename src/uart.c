#include "uart.h"

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

/* ISRs */

void UART_RecieveCallback(UART_Handle_t *handle) {
    if (handle->instance->SR & USART_SR_RXNE_Msk) { // Character recieved
        if (hnd.rx_left == 1) {
            hnd.command_ready = SET;
        }
        *hnd.cursor = handle->instance->DR; // Read data and clear RXNE bit
        hnd.cursor++;
        hnd.rx_left--;
    }
}
