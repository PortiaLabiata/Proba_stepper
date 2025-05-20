#pragma once
#include "uip.h"
#include "clock-arch.h"
#include "timer.h"
#include "uip_arp.h"
#include "utils/service.h"

#include "driver/enc28j60.h"

/* Macros */

#define ETH_INT_STATE() (GPIOB->IDR & GPIO_IDR_IDR4_Msk)

enum Net_err {
    NET_ERR_OK,
    NET_ERR_PORT
};

typedef enum Net_err Net_err_t;
extern struct timer periodic_timer, arp_timer;

int net_init(void);
void test_appcall(void);
void dev_send(void);
