#include "uart.h"

/* IO */

uint8_t UART_Transmit(uint8_t *pData, uint32_t size, int timeout) {
    uint32_t ms_start = Get_CurrentTick();

    for (uint8_t *data = pData; data - pData < size; data++) {
        while (!(USART1->SR & USART_SR_TXE_Msk)) {
            if (timeout != MAX_TIMEOUT && Get_CurrentTick() - ms_start > timeout) {
                return RESET;
            }
        } // Wait until TXE bit is set.
        USART1->DR = *data; // Transmit
    }
    while (!(USART1->SR & USART_SR_TC_Msk)) {} // Wait until TC flag is set
    return SET;
}

uint8_t UART_Recieve(uint8_t *pData, uint32_t size) {
    handle.rx_left = size;
    handle.cursor = pData;
    return SET;
}

/* ISRs */

void USART1_IRQHandler(void) {
    if (USART1->SR & USART_SR_RXNE_Msk) { // Character recieved
        if (handle.rx_left == 1) {
            handle.command_ready = SET;
        }
        *handle.cursor = USART1->DR; // Read data and clear RXNE bit
        handle.cursor++;
        handle.rx_left--;
    }
}
