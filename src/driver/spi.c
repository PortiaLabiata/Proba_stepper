#include "driver/spi.h"

SPI_Status_t SPI_SendData(uint8_t *data, uint32_t size) {
    for (uint8_t *value = data; value - data < size; value++) {
        while (!(SPI1->SR & SPI_SR_TXE_Msk)) {
            __NOP();
        }
        SPI1->DR = *value;
    }
    while (!(SPI1->SR & SPI_SR_TXE_Msk)) {
        __NOP();
    }
    return SPI_OK;
}

SPI_Status_t SPI_ReadData(uint8_t *data, uint32_t size) {
    for (uint8_t *value = data; value - data < size; value++) {
        while (!(SPI1->SR & SPI_SR_RXNE_Msk)) {
            __NOP();
        }
        *value = SPI1->DR; // When in reception, MSB is forced to zero
    }
    while (!(SPI1->SR & SPI_SR_RXNE_Msk)) {
        __NOP();
    }
    return SPI_OK;
}