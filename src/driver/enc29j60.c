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
    return SPI_OK;
}

SPI_Status_t ENC_WritePhyReg(uint8_t address, uint8_t value) {
    SPI_START_OP();
    ENC_SEL_BANK2();
    ENC_WriteReg(MIREGADR, address);
    ENC_WriteReg(MIWRL, value & 0xFF);
    ENC_WriteReg(MIWRH, value >> 8);

    ENC_SEL_BANK3();
    uint8_t rx_buffer = 0;
    ENC_ReadReg(MISTAT, &rx_buffer);
    while (!(rx_buffer & MISTAT_BUSY)) {
        ENC_ReadReg(MISTAT, &rx_buffer);
    }
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
    ENC_BitSet(ERXFCON, ERXFCON_BCEN | ERXFCON_CRCEN | ERXFCON_UCEN);

    ENC_SEL_BANK2();
    ENC_BitSet(MACON1, MACON1_MARXEN);
    // Full-duplex or not?
#ifdef ETH_FULL_DPX
    ENC_BitSet(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX);
    ENC_WriteReg(MABBIPG, 0x15);
#else
    ENC_BitSet(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
    ENC_WriteReg(MABBIPG, 0x12);
    ENC_WriteReg(MAIPGH, 0x0C);
#endif
    ENC_BitSet(MACON4, MACON4_DEFER);
    ENC_WriteReg(MAMXFLL, ETH_MAX_FRAME_LEN & 0xFF);
    ENC_WriteReg(MAMXFLH, ETH_MAX_FRAME_LEN >> 8);
    ENC_WriteReg(MAIPGL, 0x15);

    ENC_SEL_BANK3();
    ENC_WriteReg(MAADR1, MAC1);
    ENC_WriteReg(MAADR2, MAC2);
    ENC_WriteReg(MAADR3, MAC3);
    ENC_WriteReg(MAADR4, MAC4);
    ENC_WriteReg(MAADR5, MAC5);
    ENC_WriteReg(MAADR6, MAC6);

    //ENC_WritePhyReg()
    return SPI_OK;
}