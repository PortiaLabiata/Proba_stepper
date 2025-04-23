#include "utils/service.h"

/**
 * \brief Turns character into decimal number in the dumbest way possible.
 * \param[in] c Character.
 * \returns Number.
 */
int atoi(char *c) {
    if (*c < '0' || *c > '9') return -1;
    return (int)(*c - '0');
}

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