#include "core/system.h"

/* SysTick configuration */

uint32_t _current_ticks; // Current SysTick ticks, counted from startup.

/* Clock constants, will be calculated later */

uint32_t _sysclk_freq = 0;
uint32_t _ahb_freq = 0;
uint32_t _pclk2_freq = 0;
uint32_t _pclk1_freq = 0;

/* System information functions */

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

/* Configuration functions */

void ClockConfig(void) {
    RCC->CR |= RCC_CR_HSION_Msk; // Starting HSI
    while (!(RCC->CR & RCC_CR_HSIRDY_Msk)) 
        __NOP();

    // Setting PLL source is unnecessary, since reset value is 0 and
    // it corresponds to HSI/2. f=48MHz
    // Settings: PLL as SYSCLK clock source, PLLMUL=12, ADC PSC=4.
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV4 | RCC_CFGR_SW_PLL | RCC_CFGR_PLLMULL12;
    RCC->CR |= RCC_CR_PLLON_Msk; // Starting PLL
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) 
        __NOP();
    _sysclk_freq = Get_SYSCLK_Freq();
    _ahb_freq = Get_AHB_Freq();
    _pclk2_freq = Get_PCLK2_Freq();
    _pclk1_freq = Get_PCLK1_Freq();
    FLASH->ACR |= FLASH_ACR_LATENCY_2; // Enable flash latency

    NVIC_SetPriorityGrouping(4);
    SysTick_Config(AHB_FREQ / 1000); // Set SysTick to fire each ms
    NVIC_EnableIRQ(SysTick_IRQn); // Enable SysTick IRQ
}

void GPIO_Config(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN_Msk; // Enable GPIOC clocking
    GPIOC->CRH |= (GPIO_MODE_OUTPUT << GPIO_CRH_MODE13_Pos); // Set mode to 2MHz
    GPIOC->CRH &= ~(GPIO_OUTPUT_PP << GPIO_CRH_CNF13_Pos); // Set mode to output PP

    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRL |= ((0b10 << GPIO_CRL_MODE6_Pos) | (0b10 << GPIO_CRL_MODE7_Pos));
    GPIOB->CRH |= ((0B10 << GPIO_CRH_MODE8_Pos) | (0b10 << GPIO_CRH_MODE9_Pos));

    GPIOB->CRL &= ~(GPIO_CRL_CNF6_Msk | GPIO_CRL_CNF7_Msk);
    GPIOB->CRH &= ~(GPIO_CRH_CNF8_Msk | GPIO_CRH_CNF9_Msk);
}

void UART_Config(void) {
    /* GPIO config */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;

    GPIOA->CRH &= ~(GPIO_CRH_MODE9_Msk | GPIO_CRH_CNF9_Msk | \
                  GPIO_CRH_MODE10_Msk | GPIO_CRH_CNF10_Msk); // Reset bits
    
    GPIOA->CRH |= (0b10 << GPIO_CRH_CNF9_Pos) | // Set GPIOA9 to AF PP
                (0b01 << GPIO_CRH_MODE9_Pos) |
                (0b01 << GPIO_CRH_CNF10_Pos); // Set GPIOA10 to input floating
    
    /* USART config */

    USART1->BRR = PCLK2_FREQ / UART_BAUD_RATE;

    USART1->CR1 |= (USART_CR1_TE | USART_CR1_RE | \
             USART_CR1_UE); // Enable UART, reciever and transmitter
    // No need to set M bit, word length=8 bit.
    USART1->CR2 |= (0b00 << USART_CR2_STOP_Pos); // 1 STOP bit, redundant.

    hnd.command_ready = RESET;
    USART1->CR1 |= USART_CR1_RXNEIE; // Enable RXNE interrupt

    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(USART1_IRQn);
}

void TIM3_Config(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // Enable clocking
    TIM3->DIER |= TIM_DIER_UIE; // Enable update event interrupt
    TIM3->CR1 &= ~(TIM_CR1_CKD_Msk);
    NVIC_SetPriority(TIM3_IRQn, 0);
    NVIC_EnableIRQ(TIM3_IRQn);
}

/* System functions */

void delay(uint32_t ms) {
    uint32_t tick_start = Get_CurrentTick();
    while (Get_CurrentTick() - tick_start < ms) {
        __NOP();
    }
}