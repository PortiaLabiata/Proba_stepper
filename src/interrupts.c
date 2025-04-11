#include "interrupts.h"

void SysTick_Handler(void) {
    _current_ticks++;
}

void USART1_IRQHandler(void) {
    UART_RecieveCallback(&hnd);
}