#pragma once
#include "uip.h"
#include "clock-arch.h"
#include "utils/service.h"

enum Net_err {
    NET_ERR_OK,
    NET_ERR_PORT
};

typedef enum Net_err Net_err_t;

int net_init(void);
int app_poll(void);
