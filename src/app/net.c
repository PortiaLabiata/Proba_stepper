#include "app/net.h"

struct netif enc28j60_netif;
ip4_addr_t ipaddr, netmask, gateway;

u32_t sys_now(void) {
    return Get_CurrentTick();
}

static err_t ll_init(struct netif *netif) {
    return ERR_OK;
}

static struct pbuf *ll_input(struct netif *netif) {
    return ERR_OK;
}

static err_t ll_output(struct netif *netif, struct pbuf *p) {
    return ERR_OK;
}

err_t ethernetif_init(struct netif *netif) {
    netif->hwaddr_len = ETH_HWADDR_LEN;
    netif->hwaddr[0] = 0x00;
    netif->hwaddr[1] = 0x80;
    netif->hwaddr[2] = 0xE1;
    netif->hwaddr[3] = 0x00;
    netif->hwaddr[4] = 0x00;
    netif->hwaddr[5] = 0x00;
    
    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_LINK_UP | NETIF_FLAG_ETHARP;
    netif->output = etharp_output;
    netif->linkoutput = ll_output;

    ll_init(&enc28j60_netif);
    return ERR_OK;

}

void net_init(void) {
    lwip_init();
    IP4_ADDR(&ipaddr, 192, 168, 1, 2);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gateway, 192, 168, 1, 1);

    netif_add(&enc28j60_netif, &ipaddr, &netmask, &gateway, NULL, ethernetif_init, \
        netif_input);
}

err_t net_poll(void) {
    sys_check_timeouts();
}