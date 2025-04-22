#include "driver/iwdg.h"

void IWDG_Config(void) {
    IWDG->KR &= ~IWDG_KR_KEY_Msk; // Reset KR
    IWDG->KR |= IWDG_UNLOCK_CONF;
    IWDG_AWAIT_READINESS();
    //IWDG->PR |= IWDG_PR_PR_2; // Set prescaler to /64
    //IWDG_AWAIT_READINESS();
    IWDG->KR &= ~IWDG_KR_KEY_Msk;
    IWDG->KR |= IWDG_START; // Start the timer
}