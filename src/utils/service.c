#include "utils/service.h"

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

/**
 * \brief Gets current SysTick tick from the global variable.
 * \returns Current tick.
 */
uint32_t Get_CurrentTick(void) {
    return _current_ticks;
}


/**
 * \brief Outputs a string through the SWO line of the SW interface.
 * \param[in] output Output string.
 * \param[in] length Length of the string.
 */
void ITM_Print(char *output, int length) {
    for (char *c = output; c - output < length; c++) {
        ITM_SendChar((uint32_t)*c);
    }
}