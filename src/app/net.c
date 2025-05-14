#include "app/net.h"

clock_time_t
clock_time(void)
{
  return Get_CurrentTick();
}

int app_poll(void) {
    if (uip_conn->lport != 502) {
        return NET_ERR_PORT;
    }
}

int net_init(void) {
    
}