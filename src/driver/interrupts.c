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

void HardFault_Handler(void) {
    __disable_irq();
    volatile struct {
        uint32_t r0;
        uint32_t r1;
        uint32_t r2;
        uint32_t r3;
        uint32_t r12;
        uint32_t lr;
        uint32_t pc;
        uint32_t psr;
    }*stack_ptr; //Указатель на текущее значение стека(SP)

 
    asm(
      "TST lr, #4 \n" //Тестируем 3ий бит указателя стека(побитовое И)
      "ITE EQ \n"   //Значение указателя стека имеет бит 3?
      "MRSEQ %[ptr], MSP  \n"  //Да, сохраняем основной указатель стека
      "MRSNE %[ptr], PSP  \n"  //Нет, сохраняем указатель стека процесса
      : [ptr] "=r" (stack_ptr)
    );

    while (1) {
        GPIOC->BSRR |= GPIO_BSRR_BS13;
        for (int i = 0; i < 1000000; i++) __NOP();
        GPIOC->BSRR |= GPIO_BSRR_BR13;
        for (int i = 0; i < 1000000; i++) __NOP();
    }
}