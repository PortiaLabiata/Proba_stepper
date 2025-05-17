#include "driver/enc28j60.h"

SPI_Status_t ENC_ReadReg(ENC_Register_t address, uint8_t *value) {
    SPI_START_OP();
    uint8_t tx_buffer = (ENC_OP_RCR << 5) | (uint8_t)address;
    SPI_Transfer(tx_buffer, value);
    SPI_Transfer(0x00, value);
    SPI_STOP_OP();
    return SPI_OK;
}

SPI_Status_t ENC_WriteReg(ENC_Register_t address, uint8_t value) {
    SPI_START_OP();
    uint8_t tx_buffer[2];
    tx_buffer[0] = (ENC_OP_WCR << 5) | (uint8_t)address;
    tx_buffer[1] = value;
    SPI_SendData(tx_buffer, 2);
    SPI_STOP_OP();
    return SPI_OK;
}