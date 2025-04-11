#pragma once
#include "main.h"

/* Callback declarations */

uint8_t UART_RecieveCallback(USART_TypeDef *usart);
uint8_t TIM_UEV_Callback(TIM_TypeDef *tim);

/* ISRs */

void SysTick_Handler(void);
void USART1_IRQHandler(void);
void TIM3_IRQHandler(void);