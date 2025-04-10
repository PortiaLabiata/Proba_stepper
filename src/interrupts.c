#include "interrupts.h"

void SysTick_Handler(void) {
    _current_ticks++;
}