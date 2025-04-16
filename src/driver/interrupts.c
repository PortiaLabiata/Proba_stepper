#include "driver/interrupts.h"

void SysTick_Handler(void) {
    _current_ticks++;
}

void USART1_IRQHandler(void) {
    if (USART1->SR & USART_SR_RXNE_Msk) {
        UART_RecieveCallback(&ctx);
    }
}

void TIM2_IRQHandler(void) {
    TIM2->SR &= ~(TIM_SR_UIF); // Reset the timer status
    /* If stepper finished running, I don't want to add another layer of wrapping */
    if (!TIM_UEV_Callback(&ctx)) { 
        TIM2->CR1 &= ~(TIM_CR1_CEN);
    }
}

/* void TIM1_UP_IRQHandler(void) {
    TIM1->SR &= ~TIM_SR_UIF;
    GPIOC->ODR ^= GPIO_ODR_ODR13;
} */