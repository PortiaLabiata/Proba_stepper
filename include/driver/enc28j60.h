#pragma once
#include "core/system.h"
#include "core/types.h"
#include "core/prefs.h"
#include "driver/spi.h"
#include "utils/service.h"

/* Constants */

#define ETH_MAC_LEN     6
#define ETH_TYPELEN_LEN 2
#define ETH_HEAD_SIZE   2*ETH_MAC_LEN + ETH_TYPELEN_LEN

#define ETH_RX_VEC_LEN  6

/* Macros */

#define ENC_SEL_BANK0() do { \
    ENC_BitClear(ECON1, ECON1_BSEL0 | ECON1_BSEL1);} while(0)

#define ENC_SEL_BANK1() do { \
    ENC_BitClear(ECON1, ECON1_BSEL0 | ECON1_BSEL1); \
    ENC_BitSet(ECON1, ECON1_BSEL0);} while(0)

#define ENC_SEL_BANK2() do { \
    ENC_BitClear(ECON1, ECON1_BSEL0 | ECON1_BSEL1); \
    ENC_BitSet(ECON1, ECON1_BSEL1);} while(0)

#define ENC_SEL_BANK3() do { \
    ENC_BitClear(ECON1, ECON1_BSEL0 | ECON1_BSEL1); \
    ENC_BitSet(ECON1, ECON1_BSEL1 | ECON1_BSEL0);} while(0)

/* Registers */

/* Ethernet Registers */
#define ERXSTL  0x08
#define ERXSTH  0x09
#define ERXNDL  0x0A
#define ERXNDH  0x0B
#define ETXSTL  0x04
#define ETXSTH  0x05
#define ETXNDL  0x06
#define ETXNDH  0x07
#define ESTAT   0x1D
#define ECON1   0x1F
#define ECON2   0x1E
#define ERXFCON 0x18
#define ERDPTL  0x00
#define ERDPTH  0x01
#define EWRPTL  0x02
#define EWRPTH  0x03
#define EIE     0x1B
#define EIR     0x1C
#define EPKTCNT 0x19

/* MAC Registers */ 
#define MACON1  0x00
#define MACON3  0x02
#define MACON4  0x03
#define MAMXFLL 0x0A
#define MAMXFLH 0x0B
#define MABBIPG 0x04
#define MAIPGL  0x06
#define MAIPGH  0x07

/* MAC address registers */
#define MAADR5  0x00
#define MAADR6  0x01
#define MAADR3  0x02
#define MAADR4  0x03
#define MAADR1  0x04
#define MAADR2  0x05

/* PHY control registers */
#define MIREGADR 0x14
#define MIWRL    0x16
#define MIWRH    0x17
#define MISTAT   0x0A

/* PHY registers */

#define PHCON1   0x00
#define PHCON2   0x10

/* Operations */

#define ENC_OP_RCR (uint8_t)0
#define ENC_OP_RBM (uint8_t)1
#define ENC_OP_WCR (uint8_t)2
#define ENC_OP_WBM (uint8_t)3
#define ENC_OP_BFS (uint8_t)0b100
#define ENC_OP_BFC (uint8_t)0b101
#define ENC_OP_SRC (uint8_t)0xFF
#define ENC_OP_BM_SUFF (uint8_t)0x1A

/* Bitmasks */

#define ESTAT_CLKRDY_Msk (uint8_t)(1 << 0)
#define ESTAT_INT        (uint8_t)(1 << 7)
#define EIE_PKTIE        (uint8_t)(1 << 6)
#define EIE_INTIE        (uint8_t)(1 << 7)
#define EIR_TXERIF       (uint8_t)(1 << 1)
#define EIR_PKTIF        (uint8_t)(1 << 6)

#define ECON1_BSEL0      (uint8_t)(1 << 0)
#define ECON1_BSEL1      (uint8_t)(1 << 1)
#define ECON1_RXEN       (uint8_t)(1 << 2)
#define ECON1_TXRTS      (uint8_t)(1 << 3)

#define ECON2_AUTOINC    (uint8_t)(1 << 7)
#define ECON2_PKTDEC     (uint8_t)(1 << 6)

#define MACON1_MARXEN    (uint8_t)(1 << 0)
#define MACON1_TXPAUS    (uint8_t)(1 << 3)
#define MACON1_RXPAUS    (uint8_t)(1 << 2)

#define MACON3_TXCRCEN   (uint8_t)(1 << 4)
#define MACON3_FULDPX    (uint8_t)(1 << 0)
#define MACON3_FRMLNEN   (uint8_t)(1 << 1)
#define MACON3_PADCFG0   (uint8_t)(1 << 5)
#define MACON3_PADCFG1   (uint8_t)(1 << 6)
#define MACON3_PADCFG2   (uint8_t)(1 << 7)

#define ERXFCON_UCEN     (uint8_t)(1 << 7)
#define ERXFCON_CRCEN    (uint8_t)(1 << 5)
#define ERXFCON_BCEN     (uint8_t)(1 << 0)

#define MACON4_DEFER     (uint8_t)(1 << 6)
#define MISTAT_BUSY      (uint8_t)(1 << 0)
#define PHCON2_HDLDIS    (uint16_t)(1 << 8)
#define PHCON1_PDPXMD    (uint16_t)(1 << 8)

/* Low-level IO */

SPI_Status_t ENC_ReadReg(uint8_t address, uint8_t *value);
SPI_Status_t ENC_WriteReg(uint8_t address, uint8_t value);
SPI_Status_t ENC_BitSet(uint8_t address, uint8_t mask);
SPI_Status_t ENC_BitClear(uint8_t address, uint8_t mask);
SPI_Status_t ENC_SoftReset(void);
SPI_Status_t ENC_WritePhyReg(uint8_t address, uint16_t value);
SPI_Status_t ENC_ReadMacMIIReg(uint8_t address, uint8_t *value);
SPI_Status_t ENC_WriteBufferMemory(uint16_t address, uint8_t *data, uint32_t size);
SPI_Status_t ENC_ReadBufferMemory(uint16_t address, uint8_t *data, uint32_t size);

/* Control */

SPI_Status_t ENC_PendClkrdy(void);
SPI_Status_t ENC_Init(void);
SPI_Status_t ENC_SendPacket(uint8_t *dst_addr, uint8_t *type_len, uint8_t *data, uint32_t size);
uint32_t ENC_RecievePacket(uint8_t data[]);
SPI_Status_t ENC_SendPacketRaw(uint8_t *data, uint32_t size);
