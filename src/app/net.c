#include "app/net.h"

struct timer periodic_timer, arp_timer;

clock_time_t
clock_time(void)
{
  return Get_CurrentTick();
}

int net_init(void) {
    struct uip_eth_addr mac = {
        {ETH_MAC1, ETH_MAC2, ETH_MAC3, ETH_MAC4, ETH_MAC5, ETH_MAC6}
    };
    uip_ipaddr_t ipaddr;

    timer_set(&periodic_timer, CLOCK_SECOND / 2);
    timer_set(&arp_timer, CLOCK_SECOND * 10);

    uip_init();
    uip_arp_init();
    ENC_Init();

    uip_setethaddr(mac);

    uip_ipaddr(ipaddr, 192, 168, 31, 211);
    uip_sethostaddr(ipaddr);
    uip_ipaddr(ipaddr, 192, 168, 31, 1);
    uip_setdraddr(ipaddr);
    uip_ipaddr(ipaddr, 255, 255, 255, 0);
    uip_setnetmask(ipaddr);
    return (int)NET_ERR_OK;
}

void test_appcall(void) {
  ;
}

void dev_send(void) {
  ENC_SendPacketRaw(uip_buf, uip_len);
}