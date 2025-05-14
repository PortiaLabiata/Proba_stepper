#pragma once
#include <stm32f103x6.h>
#define UIP_APPCALL() app_poll()

/* Forward declarations */

extern int app_poll(void);

/* uIP declarations */

struct mb_app_state {
    uint32_t dummy;
};

typedef struct mb_app_state uip_tcp_appstate_t;