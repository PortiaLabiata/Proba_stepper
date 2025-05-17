#pragma once
/* User configuration */

//#define IWDG_ENABLE
#define STARTUP_BLINK_ENABLE
#define SW_LOGGING_ENABLE

#define UART_BAUD_RATE   115200
#define MB_BAUD_RATE     9600

/* ENC28J60 preferences */

#define ETH_FULL_DPX
#define RX_BUFFER_START     (uint16_t)0
#define RX_BUFFER_SIZE      (uint16_t)6144 // 6KByte
#define ETH_MAX_FRAME_LEN   (uint16_t)1518

#define MAC1    (uint8_t)0x00
#define MAC2    (uint8_t)0x01
#define MAC3    (uint8_t)0x02
#define MAC4    (uint8_t)0x03
#define MAC5    (uint8_t)0x04
#define MAC6    (uint8_t)0x05