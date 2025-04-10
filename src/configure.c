#include "configure.h"

uint32_t _sysclk_freq = 0;
uint32_t _ahb_freq = 0;
uint32_t _pclk2_freq = 0;

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

    handle.command_ready = RESET;
    USART1->CR1 |= USART_CR1_RXNEIE; // Enable RXNE interrupt

    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(USART1_IRQn);
}