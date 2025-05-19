#pragma once
#include "core/system.h"
#include "core/types.h"

/* Callback forward declarations */

uint8_t UART_RecieveCallback(System_Context_t *ctx);
uint8_t TIM_UEV_Callback(System_Context_t *ctx);

/* ISRs */

void SysTick_Handler(void);
void USART1_IRQHandler(void);
void TIM2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void HardFault_Handler(void);

extern volatile uint8_t ppend;