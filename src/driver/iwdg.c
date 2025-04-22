#include "driver/iwdg.h"
#ifdef USE_IWDG

void IWDG_Config(void) {
    IWDG->KR &= ~IWDG_KR_KEY_Msk; // Reset KR
    IWDG->KR |= IWDG_UNLOCK_CONF;
    IWDG_AWAIT_READINESS();
    //IWDG->PR |= (IWDG_PR_PR_2 | IWDG_PR_PR_1 | IWDG_PR_PR_0); // Set prescaler to /128
    //IWDG_AWAIT_READINESS();
    IWDG->KR &= ~IWDG_KR_KEY_Msk;
    IWDG->KR |= IWDG_START; // Start the timer
}

#else // In case if I don't need IWDG

void IWDG_Config(void) {};

#endif