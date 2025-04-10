#include "service.h"

/* Global variables */
uint32_t _current_ticks;

uint32_t Get_SYSCLK_Freq(void) {
    uint32_t pmul;
    switch (RCC->CFGR & (3UL << RCC_CFGR_SWS_Pos)) {
        case RCC_CFGR_SWS_HSI:
            return HSI_FREQ;
        case RCC_CFGR_SWS_HSE:
            return HSE_FREQ;
        case RCC_CFGR_SWS_PLL:
            pmul = (RCC->CFGR >> RCC_CFGR_PLLMULL_Pos) & 0b1111;
            return (pmul + 2)*HSI_FREQ/2;
        default:
            return 0; // Something went horribly wrong
    }
}

uint32_t Get_AHB_Freq(void) {
    switch (RCC->CFGR & (0b1111 << RCC_CFGR_HPRE_Pos)) {
        case RCC_CFGR_HPRE_DIV1:
            return SYSCLK_FREQ / 1;
        case RCC_CFGR_HPRE_DIV2:
            return SYSCLK_FREQ / 2;
        case RCC_CFGR_HPRE_DIV4:
            return SYSCLK_FREQ / 4;
        case RCC_CFGR_HPRE_DIV8:
            return SYSCLK_FREQ / 8;
        case RCC_CFGR_HPRE_DIV16:
            return SYSCLK_FREQ / 16;
        case RCC_CFGR_HPRE_DIV64:
            return SYSCLK_FREQ / 64;
        case RCC_CFGR_HPRE_DIV128:
            return SYSCLK_FREQ / 128;
        case RCC_CFGR_HPRE_DIV256:
            return SYSCLK_FREQ / 256;
        case RCC_CFGR_HPRE_DIV512:
            return SYSCLK_FREQ / 512;
        default:
            return 0; // Something went horribly wrong
    }
}

uint32_t Get_PCLK2_Freq(void) {
    switch (RCC->CFGR & (0b111 << RCC_CFGR_PPRE2_Pos)) {
        case RCC_CFGR_PPRE2_DIV1:
            return AHB_FREQ / 1;
        case RCC_CFGR_PPRE2_DIV2:
            return AHB_FREQ / 2;
        case RCC_CFGR_PPRE2_DIV4:
            return AHB_FREQ / 4;
        case RCC_CFGR_PPRE2_DIV8:
            return AHB_FREQ / 8;
        case RCC_CFGR_PPRE2_DIV16:
            return AHB_FREQ / 16;
        default:
            return 0;
    }
}

uint32_t Get_PCLK1_Freq(void) {
    switch (RCC->CFGR & (0b111 << RCC_CFGR_PPRE1_Pos)) {
        case RCC_CFGR_PPRE1_DIV1:
            return AHB_FREQ / 1;
        case RCC_CFGR_PPRE1_DIV2:
            return AHB_FREQ / 2;
        case RCC_CFGR_PPRE1_DIV4:
            return AHB_FREQ / 4;
        case RCC_CFGR_PPRE1_DIV8:
            return AHB_FREQ / 8;
        case RCC_CFGR_PPRE1_DIV16:
            return AHB_FREQ / 16;
        default:
            return 0;
    }
}

uint32_t Get_CurrentTick(void) {
    return _current_ticks;
}

void delay(uint32_t ms) {
    uint32_t tick_start = Get_CurrentTick();
    while (Get_CurrentTick() - tick_start < ms) {
        __NOP();
    }
}