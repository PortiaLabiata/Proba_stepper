#ifndef LWIOPTS_H_
#define LWIOPTS_H_

//#include "sys_arch.h"

#define NO_SYS          1

#define LWIP_RAW        1
#define LWIP_DHCP       0
#define LWIP_SNMP       0
#define LWIP_IGMP       0
#define LWIP_DNS        0
#define LWIP_TCP        1
#define LWIP_IP         1
#define LWIP_ARP        1
#define LWIP_ICMP       1
#define LWIP_PPP_API    0
#define LWIP_IPV6       0
#define LWIP_SNMP       0
#define LWIP_AUTOIP     0
#define LWIP_NETIF_API  0

#define IP_FORWARD      0
#define LWIP_STATS      0

#define LWIP_SOCKET     0
#define LWIP_NETCONN    0

#define MEM_SIZE            (3*1024)
#define MEMP_NUM_PBUF       8
#define PBUF_POOL_SIZE      8
#define PBUF_POOL_BUFSIZE   256
#define TCP_WND             (2 * TCP_MSS)
#define TCP_SND_BUF         (2 * TCP_MSS)
#define MEMP_NUM_TCP_PCB    1
#define MEMP_NUM_UDP_PCB    1

#endif