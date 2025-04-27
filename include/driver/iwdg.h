#include "main.h"
#include "core/system.h"
#include "core/types.h"

/* Macros */

#define IWDG_UNLOCK_CONF 0x5555UL
#define IWDG_START       0xCCCCUL
#define IWDG_RESET       0xAAAAUL

#ifdef IWDG_ENABLE
#define IWDG_RELOAD() do { \
    IWDG->KR &= ~IWDG_KR_KEY_Msk; \
    IWDG->KR |= IWDG_RESET; \
} while (0)
#else
#define IWDG_RELOAD() __NOP()
#endif

#define IWDG_AWAIT_READINESS() while (IWDG->SR & IWDG_SR_PVU_Msk) __NOP();

/* Typedefs */

typedef enum {
    IWDG_PSC_4,
    IWDG_PSC_8,
    IWDG_PSC_16,
    IWDG_PSC_32,
    IWDG_PSC_64,
    IWDG_PSC_128,
    IWDG_PSC_256,
    IWDG_PSC_256_1
} IWDG_Prescaler_t;