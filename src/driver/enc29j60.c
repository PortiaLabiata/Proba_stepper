#include "driver/enc28j60.h"

// Dummy variable
static uint8_t dummy = 0;

SPI_Status_t ENC_ReadReg(uint8_t address, uint8_t *value) {
    SPI_START_OP();
    uint8_t tx_buffer = (ENC_OP_RCR << 5) | (uint8_t)address;
    SPI_Transfer(tx_buffer, value);
    SPI_Transfer(0x00, value);
    SPI_STOP_OP();
    return SPI_OK;
}

SPI_Status_t ENC_WriteReg(uint8_t address, uint8_t value) {
    SPI_START_OP();
    uint8_t tx_buffer[2];
    tx_buffer[0] = (ENC_OP_WCR << 5) | (uint8_t)address;
    tx_buffer[1] = value;
    SPI_Transfer(tx_buffer[0], &dummy);
    SPI_Transfer(tx_buffer[1], &dummy);
    SPI_STOP_OP();
    return SPI_OK;
}

SPI_Status_t ENC_BitSet(uint8_t address, uint8_t mask) {
    SPI_START_OP();
    SPI_Transfer((ENC_OP_BFS << 5) | (uint8_t)address, &dummy);
    SPI_Transfer(mask, &dummy);
    SPI_STOP_OP();
    return SPI_OK;
}

SPI_Status_t ENC_BitClear(uint8_t address, uint8_t mask) {
    SPI_START_OP();
    SPI_Transfer((ENC_OP_BFC << 5) | (uint8_t)address, &dummy);
    SPI_Transfer(mask, &dummy);
    SPI_STOP_OP();
    return SPI_OK;
}

SPI_Status_t ENC_SoftReset(void) {
    SPI_START_OP();
    SPI_Transfer(ENC_OP_SRC, &dummy);
    SPI_STOP_OP();
}

SPI_Status_t ENC_PendClkrdy(void) {
    uint8_t rx_buffer;
    ENC_ReadReg(ESTAT, &rx_buffer);
    while (!(rx_buffer & ESTAT_CLKRDY_Msk)) {
        delay(1);
        ENC_ReadReg(ESTAT, &rx_buffer);
    }
    return SPI_OK;
}

SPI_Status_t ENC_Init(void) {
    ENC_PendClkrdy();
    ENC_SoftReset();

    ENC_SEL_BANK0();
    ENC_WriteReg(ERXSTL, RX_BUFFER_START & 0xFF);
    ENC_WriteReg(ERXSTH, RX_BUFFER_START >> 8);
    ENC_WriteReg(ERXNDL, (RX_BUFFER_START + RX_BUFFER_SIZE) & 0xFF);
    ENC_WriteReg(ERXNDH, (RX_BUFFER_START + RX_BUFFER_SIZE) >> 8); // 6KByte buffer

    ENC_SEL_BANK2();
    ENC_BitSet(MACON1, MACON1_MARXEN);
    // Full-duplex or not?
#ifdef ETH_FULL_DPX
    ENC_BitSet(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX);
#else
    ENC_BitSet(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
#endif
}