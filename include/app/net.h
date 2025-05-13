#include "lwip/tcp.h"
#include "lwip/timeouts.h"
#include "lwip/init.h"
#include "lwip/etharp.h"

#include "utils/service.h"

err_t net_poll(void);
err_t ethernetif_init(struct netif *netif);
void net_init(void);