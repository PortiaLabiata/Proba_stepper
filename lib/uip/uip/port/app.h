#pragma once
#include <stm32f103x6.h>
#define UIP_APPCALL() uip_modbus_appcall()

/* Forward declarations */

extern void uip_modbus_appcall(void);

/* uIP declarations */

struct mb_app_state {
    uint32_t dummy;
};

typedef struct mb_app_state uip_tcp_appstate_t;