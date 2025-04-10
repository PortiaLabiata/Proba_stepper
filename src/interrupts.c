#include "interrupts.h"

void SysTick_Handler(void) {
    _current_ticks++;
}

void USART1_IRQHandler(void) {
    if (USART1->SR & USART_SR_RXNE_Msk) { // Character recieved
        handle.command = USART1->DR; // Read data and clear RXNE bit
        handle.command_ready = SET;
    }
}