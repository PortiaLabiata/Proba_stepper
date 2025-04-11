#include "interrupts.h"

void SysTick_Handler(void) {
    _current_ticks++;
}

void USART1_IRQHandler(void) {
    UART_RecieveCallback(USART1);
}

void TIM3_IRQHandler(void) {
    TIM3->SR &= ~(TIM_SR_UIF); // Reset the timer status
    TIM_UEV_Callback(TIM3);
}