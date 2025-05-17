#pragma once
#include "core/system.h"
#include "core/types.h"
#include "core/prefs.h"
#include "driver/spi.h"
#include "utils/service.h"

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
#define ESTAT   0x1D
#define ECON1   0x1F
#define ERXFCON 0x18

/* MAC Registers */ 
#define MACON1  0x00
#define MACON3  0x02
#define MACON4  0x03
#define MAMXFLL 0x0A
#define MAMXFLH 0x0B
#define MABBIPG 0x04
#define MAIPGL  0x06

/* MAC address registers */
#define MAADR5 0x00
#define MAADR6 0x01
#define MAADR3 0x02
#define MAADR4 0x03
#define MAADR1 0x04
#define MAADR2 0x05

/* PHY control registers */
#define MIREGADR 0x14
#define MIWRL    0x16
#define MIWRH    0x17
#define MISTAT   0x0A

/* PHY registers */

/* Operations */

#define ENC_OP_RCR (uint8_t)0
#define ENC_OP_RBM (uint8_t)1
#define ENC_OP_WCR (uint8_t)2
#define ENC_OP_WBM (uint8_t)3
#define ENC_OP_BFS (uint8_t)4
#define ENC_OP_BFC (uint8_t)5
#define ENC_OP_SRC (uint8_t)0xFF

/* Bitmasks */

#define ESTAT_CLKRDY_Msk (uint8_t)1
#define ECON1_BSEL0      (uint8_t)1
#define ECON1_BSEL1      (uint8_t)2
#define MACON1_MARXEN    (uint8_t)1

#define MACON3_TXCRCEN   (uint8_t)0b00010000
#define MACON3_FULDPX    (uint8_t)0b00000001
#define MACON3_FRMLNEN   (uint8_t)0b00000010
#define MACON3_PADCFG0   (uint8_t)0b00100000
#define MACON3_PADCFG1   (uint8_t)0b01000000
#define MACON3_PADCFG2   (uint8_t)0b10000000

#define ERXFCON_UCEN     (uint8_t)0b10000000
#define ERXFCON_CRCEN    (uint8_t)0b00100000
#define ERXFCON_BCEN     (uint8_t)0b00000001

#define MACON4_DEFER     (uint8_t)0b01000000
#define MISTAT_BUSY      (uint8_t)0b00000001

/* Low-level IO */

SPI_Status_t ENC_ReadReg(uint8_t address, uint8_t *value);
SPI_Status_t ENC_WriteReg(uint8_t address, uint8_t value);
SPI_Status_t ENC_BitSet(uint8_t address, uint8_t mask);
SPI_Status_t ENC_BitClear(uint8_t address, uint8_t mask);
SPI_Status_t ENC_SoftReset(void);
SPI_Status_t ENC_WritePhyReg(uint8_t address, uint8_t value);

/* Control */

SPI_Status_t ENC_PendClkrdy(void);
SPI_Status_t ENC_Init(void);
