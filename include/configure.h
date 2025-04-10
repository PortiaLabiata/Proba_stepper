#pragma once
#include "main.h"
#include "service.h"

/* Magic numbers */

#define GPIO_MODE_OUTPUT 0b10UL
#define GPIO_OUTPUT_PP   0b00UL
#define GPIO_OUTPUT_OD   0b01UL
#define GPIO_INPUT_P     0b10UL

#define HSI_FREQ         8000000 // Hz
#define HSE_FREQ         16000000 // I don't have it, may vary
/* Since frequency is not intended to change during runtime, this approach is better,
since it calculates the f-s only once on startup. */
#define SYSCLK_FREQ      _sysclk_freq
#define AHB_FREQ         _ahb_freq
#define PCLK2_FREQ       _pclk2_freq
#define PCLK1_FREQ       _pclk1_freq

/* Global definitions */

extern uint32_t _sysclk_freq;
extern uint32_t _ahb_freq;
extern uint32_t _pclk2_freq;
extern uint32_t _pclk1_freq;

/* General configuration */

void ClockConfig(void);
void GPIO_Config(void);
void UART_Config(void);
void TIM3_Config(void);
