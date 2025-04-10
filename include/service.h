#pragma once
#include "main.h"
#include "configure.h"

/* Global variables */
extern uint32_t _current_ticks;

uint32_t Get_SYSCLK_Freq(void);
uint32_t Get_AHB_Freq(void);
uint32_t Get_PCLK2_Freq(void);
uint32_t Get_PCLK1_Freq(void);
uint32_t Get_CurrentTick(void);
void delay(uint32_t ms);

int atoi(char *c);
