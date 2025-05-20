#pragma once
/* User configuration */

//#define IWDG_ENABLE
#define STARTUP_BLINK_ENABLE    1
#define SW_LOGGING_ENABLE

#define UART_BAUD_RATE   115200
#define MB_BAUD_RATE     9600

/* ENC28J60 preferences */

#define ETH_FULL_DPX        0
#define ETH_RX_BUFFER_START (uint16_t)0
#define ETH_RX_BUFFER_SIZE  (uint16_t)6144 // 6KByte
#define ETH_MAX_FRAME_LEN   (uint16_t)1518
#define ETH_TX_BUFFER_START (uint16_t)(ETH_RX_BUFFER_START + ETH_RX_BUFFER_SIZE) // Even address
#define ETH_TX_BUFFER_SIZE  (uint16_t)2047 // 2KBytes - 1 byte

#define ETH_MAC1    (uint8_t)0x00
#define ETH_MAC2    (uint8_t)0x01
#define ETH_MAC3    (uint8_t)0x02
#define ETH_MAC4    (uint8_t)0x03
#define ETH_MAC5    (uint8_t)0x04
#define ETH_MAC6    (uint8_t)0x05

/* Modbus Preferences */

#define MB_PORT 502