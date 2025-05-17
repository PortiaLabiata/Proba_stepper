#pragma once
#include "core/system.h"
#include "core/types.h"
#include "core/prefs.h"
#include "driver/spi.h"

#define ENC_OP_RCR (uint8_t)0
#define ENC_OP_RBM (uint8_t)1
#define ENC_OP_WCR (uint8_t)2
#define ENC_OP_WBM (uint8_t)3
#define ENC_OP_BFS (uint8_t)4
#define ENC_OP_BFC (uint8_t)5
#define ENC_OP_SRC (uint8_t)6

/* Registers */

typedef enum {
    ERXSTL=0x08,
    ERXSTH=0x09,
    ERXNDL=0x0A,
    ERXNDH=0x0B,
    ESTAT=0x19
} ENC_Register_t;

/* Prototypes */

SPI_Status_t ENC_ReadReg(ENC_Register_t address, uint8_t *value);
SPI_Status_t ENC_WriteReg(ENC_Register_t address, uint8_t value);
