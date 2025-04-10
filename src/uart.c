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

uint8_t ProcessCommand(void) {
    UART_Transmit(&handle.command, 1, MAX_TIMEOUT);
    handle.command_ready = RESET;
    return SET;
}
