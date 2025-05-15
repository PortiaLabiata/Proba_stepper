#include "app/net.h"

clock_time_t
clock_time(void)
{
  return Get_CurrentTick();
}

/**
 * \brief Here will be low-level initialization: ENC28J60, timers etc.
 */
static int app_init(void) {
    return 0;
}

int net_init(void) {
    struct uip_eth_addr mac = {
        {0x00, 0x01, 0x02, 0x03, 0x04, 0x00}
    };
    uip_ipaddr_t ipaddr;
    struct timer periodic_timer, arp_timer;

    timer_set(&periodic_timer, CLOCK_SECOND / 2);
    timer_set(&arp_timer, CLOCK_SECOND * 10);

    uip_init();
    uip_arp_init();
    app_init();

    uip_setethaddr(mac);

    uip_ipaddr(ipaddr, 192, 168, 1, 2);
    uip_sethostaddr(ipaddr);
    uip_ipaddr(ipaddr, 192, 168, 1, 1);
    uip_setdraddr(ipaddr);
    uip_ipaddr(ipaddr, 255, 255, 255, 0);
    uip_setnetmask(ipaddr);
    return (int)NET_ERR_OK;
}