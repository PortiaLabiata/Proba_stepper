#pragma once
#include "uip.h"
#include "clock-arch.h"
#include "timer.h"
#include "uip_arp.h"
#include "utils/service.h"

#include "driver/enc28j60.h"

enum Net_err {
    NET_ERR_OK,
    NET_ERR_PORT
};

typedef enum Net_err Net_err_t;

int net_init(void);
