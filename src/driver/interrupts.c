#include "driver/interrupts.h"

void SysTick_Handler(void) {
    _current_ticks++;
}

void USART1_IRQHandler(void) {
    if (USART1->SR & USART_SR_RXNE_Msk) {
        UART_RecieveCallback(&ctx);
    }
}

void TIM3_IRQHandler(void) {
    TIM3->SR &= ~(TIM_SR_UIF); // Reset the timer status
    /* If stepper finished running, I don't want to add another layer of wrapping */
    if (!TIM_UEV_Callback(&ctx)) { 
        TIM3->CR1 &= ~(TIM_CR1_CEN);
    }
}