#include "core/system.h"

/* SysTick configuration */

volatile uint32_t _current_ticks; // Current SysTick ticks, counted from startup.

/* Clock constants, will be calculated later */

uint32_t _sysclk_freq = 0;
uint32_t _ahb_freq = 0;
uint32_t _pclk2_freq = 0;
uint32_t _pclk1_freq = 0;

/* System information functions */

/**
 * \brief Calculates SYSCLK frequency based on RCC registers content. Is supposed to 
 * only run once at the start of the program.
 * \returns Frequency in hertz.
 */
static uint32_t Get_SYSCLK_Freq(void) {
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

/**
 * \brief Calculates AHB frequency based on RCC registers content. Is supposed to 
 * only run once at the start of the program.
 * \returns Frequency in hertz.
 */
static uint32_t Get_AHB_Freq(void) {
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

/**
 * \brief Calculates PCLK2 frequency based on RCC registers content. Is supposed to 
 * only run once at the start of the program.
 * \returns Frequency in hertz.
 */
static uint32_t Get_PCLK2_Freq(void) {
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

/**
 * \brief Calculates PCLK1 frequency based on RCC registers content. Is supposed to 
 * only run once at the start of the program.
 * \returns Frequency in hertz.
 */
static uint32_t Get_PCLK1_Freq(void) {
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


/**
 * \brief Gets current SysTick tick from the global variable.
 * \returns Current tick.
 */
uint32_t Get_CurrentTick(void) {
    return _current_ticks;
}

/* Configuration functions */

/**
 * \brief Low-level configuration of RCC.
 */
void ClockConfig(void) {
    RCC->CR |= RCC_CR_HSION_Msk; // Starting HSI
    while (!(RCC->CR & RCC_CR_HSIRDY_Msk)) 
        __NOP();

    // Setting PLL source is unnecessary, since reset value is 0 and
    // it corresponds to HSI/2. f=48MHz
    // Settings: PLL as SYSCLK clock source, PLLMUL=12, ADC PSC=4.

    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV4 | RCC_CFGR_SW_PLL | RCC_CFGR_PLLMULL12;
    RCC->CR |= RCC_CR_PLLON; // Starting PLL
    while (!(RCC->CR & RCC_CR_PLLRDY)) 
        __NOP();
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

/**
 * \brief Low-level configuration of GPIO, excluding those that UART uses.
 */
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

/**
 * \brief Low-level configuration of UART, including GPIO.
 */
void UART_Config(void) {
    RCC->APB2ENR |= (RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN);

    // 1. Clear remap bit first (avoid glitches)
    AFIO->MAPR &= ~AFIO_MAPR_USART1_REMAP;  
    // 2. Enable remap: PB6=TX, PB7=RX
    AFIO->MAPR |= AFIO_MAPR_USART1_REMAP;    

    // Configure PB6 (TX) as Alternate Function Push-Pull Output
    GPIOB->CRL &= ~(GPIO_CRL_CNF6 | GPIO_CRL_MODE6);
    GPIOB->CRL |= (GPIO_CRL_CNF6_1 | GPIO_CRL_MODE6_1);  // 50MHz output

    // Configure PB7 (RX) as Input Floating (no pull-up/pull-down)
    GPIOB->CRL &= ~(GPIO_CRL_CNF7 | GPIO_CRL_MODE7);
    GPIOB->CRL |= GPIO_CRL_CNF7_1;  // Input floating


    /* USART config */

    USART1->BRR = PCLK2_FREQ / UART_BAUD_RATE;

    USART1->CR1 |= (USART_CR1_TE | USART_CR1_RE | \
             USART_CR1_UE); // Enable UART, reciever and transmitter
    // No need to set M bit, word length=8 bit.
    //USART1->CR2 |= (0b00 << USART_CR2_STOP_Pos); // 1 STOP bit, redundant.

    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(USART1_IRQn);
}

/**
 * \brief Configures timers for dynamic update of GPIO output values in UEV ISR.
 * This approach is perhaps the simplest and it only uses one timer, but timer UEV interrupt
 * will fire approx. every 10 ms, which is not optimal.
 */
void TIM_Config_Dynamic(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable clocking
    TIM2->DIER |= TIM_DIER_UIE; // Enable update event interrupt
    TIM2->CR1 &= ~(TIM_CR1_CKD_Msk);
    NVIC_SetPriority(TIM2_IRQn, 0);
    NVIC_EnableIRQ(TIM2_IRQn);
}

/**
 * 
 */
void TIM_Config_Static(void) {

    /* TIM1 CONFIGURATION */

    /* GPIO configuration */

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH &= ~GPIO_CRH_CNF8_Msk;
    GPIOA->CRH |= GPIO_CRH_CNF8_1;
    GPIOA->CRH &= ~GPIO_CRH_CNF11_Msk;
    GPIOA->CRH |= GPIO_CRH_CNF11_1;

    GPIOA->CRH |= GPIO_CRH_MODE8_1;
    GPIOA->CRH |= GPIO_CRH_MODE11_1;

    /* Configure timer itself */
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    TIM1->CR1 &= ~TIM_CR1_CKD;
    TIM1->CR1 |= (TIM_CR1_CKD_0 | TIM_CR1_CKD_1);
    TIM1->PSC = 399;
    TIM1->ARR = 1199; // Period of the actual PWM freq
    TIM1->RCR = 255; // Set repetition counter to 255
    //TIM1->EGR |= TIM_EGR_UG; // ??

    /* Channel 1 */
    TIM1->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2); // Set mode to PWM1
    TIM1->CCR1 = TIM1->ARR / 4;
    TIM1->CCER |= TIM_CCER_CC1E; // Enable Channel 1

    /* Channel 2 */
    TIM1->CCMR1 |= (TIM_CCMR2_OC4M_0 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2); // Set mode to PWM2
    TIM1->CCR1 = TIM1->ARR / 4;
    TIM1->CCER |= TIM_CCER_CC1E;

    /* Channel 4 */
    TIM1->CCMR2 |= (TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2); // PWM2
    TIM1->CCR4 = 3*TIM1->ARR / 4;
    TIM1->CCER |= TIM_CCER_CC4E; // Enable Channel 4

    TIM1->DIER |= TIM_DIER_UIE; // Enable UEV ISR
    TIM1->BDTR |= TIM_BDTR_MOE; // Enable the damn main outputs
    TIM1->CR2 |= (TIM_CR2_MMS_0 | TIM_CR2_MMS_2); // Set master output to CC2
    TIM1->EGR |= TIM_EGR_UG; // Update timer

    NVIC_SetPriority(TIM1_UP_IRQn, 0);
    NVIC_EnableIRQ(TIM1_UP_IRQn);
    // Jesus, that's a long section

    /* TIM2 CONFIGURATION */

    /* GPIO Configuration, channels 2 and 3!!! */
    GPIOA->CRL &= ~GPIO_CRL_CNF1_Msk;
    GPIOA->CRL &= ~GPIO_CRL_CNF2_Msk;
    GPIOA->CRL |= GPIO_CRL_CNF1_1;
    GPIOA->CRL |= GPIO_CRL_CNF2_1;

    GPIOA->CRL |= GPIO_CRL_MODE1_1;
    GPIOA->CRL |= GPIO_CRL_MODE2_1;

    GPIOA->CRL &= ~GPIO_CRL_CNF0;  // Clear CNF0 bits
    GPIOA->CRL |= GPIO_CRL_CNF0_0;  // Floating input (CNF=01)
    GPIOA->CRL &= ~GPIO_CRL_MODE0_Msk;

    /* Configure timer itself */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable clocking
    TIM2->CR1 &= ~TIM_CR1_CKD_Msk;
    TIM2->CR1 |= (TIM_CR1_CKD_0 | TIM_CR1_CKD_1);
    TIM2->ARR = 599;
    TIM2->PSC = 399;

    TIM2->SMCR &= ~TIM_SMCR_SMS_Msk; // Set TIM1 as master
    //TIM2->SMCR |= (TIM_SMCR_TS_0 | TIM_SMCR_TS_1 | TIM_SMCR_TS_2); // Set ETR as trigger source
    // ITR0 set as trigger source
    TIM2->SMCR |= TIM_SMCR_ETP; // Invert ETR polarity
    TIM2->SMCR |= TIM_SMCR_SMS_2; // Set TIM2 in slave reset mode

    /* Channel 2 */
    TIM2->CCMR1 |= (TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2); // Set mode to PWM1
    TIM2->CCR2 = TIM2->ARR / 2;
    TIM2->CCER |= TIM_CCER_CC2E; // Enable Channel 2

    /* Channel 3 */
    TIM2->CCMR2 |= (TIM_CCMR2_OC3M_0 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2); // PWM2
    TIM2->CCR3 = TIM2->ARR / 2;
    TIM2->CCER |= TIM_CCER_CC3E; // Enable Channel 3
    TIM2->CR1 |= TIM_CR1_CEN; // Enable timer 2, first bc won't start wo tim1
    TIM1->CR1 |= TIM_CR1_CEN; // Enable timer 1
}

/* System functions */

/**
 * \brief Busy-waiting loop for delays, based on SysTick ticks.
 * \param[in] ms Delay duration in ms.
 */
void delay(uint32_t ms) {
    uint32_t tick_start = Get_CurrentTick();
    while (Get_CurrentTick() - tick_start < ms) {
        __NOP();
    }
}