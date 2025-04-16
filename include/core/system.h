#pragma once
#include <stm32f103x6.h>
#include "core/types.h"

/* Magic numbers */

#define GPIO_MODE_OUTPUT 0b10UL
#define GPIO_OUTPUT_PP   0b00UL
#define GPIO_OUTPUT_OD   0b01UL
#define GPIO_INPUT_P     0b10UL

#define HSI_FREQ         8000000 // Hz
#define HSE_FREQ         16000000 // I don't have it, may vary

#define UART_BAUD_RATE   115200 

#define SYSCLK_FREQ      _sysclk_freq
#define AHB_FREQ         _ahb_freq
#define PCLK2_FREQ       _pclk2_freq
#define PCLK1_FREQ       _pclk1_freq

#ifndef NULL
    #define NULL ((void *)0)
#endif

/* Macros */

#define BIT_SET(__REG__, __MASK__) __REG__ |= MASK
#define BIT_RESET(__REG__, __MASK__) __REG__ &= ~MASK

/* Global definitions */

extern volatile uint32_t _current_ticks;

extern uint32_t _sysclk_freq;
extern uint32_t _ahb_freq;
extern uint32_t _pclk2_freq;
extern uint32_t _pclk1_freq;

/* System information functions */

uint32_t Get_CurrentTick(void);

/* Configuration functions */

uint32_t Get_TIM_Div(TIM_TypeDef *tim);

void ClockConfig(void);
void GPIO_Config(void);
void UART_Config(void);
void TIM2_Config(void);

/* System functions */

void delay(uint32_t ms);
