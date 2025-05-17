#pragma once
/* User configuration */

//#define IWDG_ENABLE
#define STARTUP_BLINK_ENABLE
#define SW_LOGGING_ENABLE

#define UART_BAUD_RATE   115200
#define MB_BAUD_RATE     9600

/* ENC28J60 preferences */

#define RX_BUFFER_START (uint16_t)0
#define RX_BUFFER_SIZE  (uint16_t)6144 // 6KByte
#define ETH_FULL_DPX