#include "driver/interrupts.h"
#include "driver/enc28j60.h"

void SysTick_Handler(void) {
    _current_ticks++;
}

void TIM2_IRQHandler(void) {
    TIM2->SR &= ~(TIM_SR_UIF); // Reset the timer status
    /* If stepper finished running, I don't want to add another layer of wrapping */
    if (!TIM_UEV_Callback(&ctx)) { 
        TIM2->CR1 &= ~(TIM_CR1_CEN);
    }
}

void EXTI3_IRQHandler(void) {
    EXTI->PR |= EXTI_PR_PR3;
    GPIOC->ODR ^= GPIO_ODR_ODR13;
    //ENC_ReadReg(ESTAT, &dummy);
    ENC_BitClear(ESTAT, ESTAT_INT);
}