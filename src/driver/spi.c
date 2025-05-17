#include "driver/spi.h"

SPI_Status_t SPI_SendData(uint8_t *data, uint32_t size) {
    for (uint8_t *value = data; value - data < size; value++) {
        while (!(SPI1->SR & SPI_SR_TXE_Msk)) {
            __NOP();
        }
        SPI1->DR = *value;
    }
    while (SPI1->SR & SPI_SR_BSY_Msk) {
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
    return SPI_OK;
}

SPI_Status_t SPI_Transfer(uint8_t tx_value, uint8_t *rx_buffer) {
    (void)SPI1->DR;
    while (!(SPI1->SR & SPI_SR_TXE_Msk)) ;
    SPI1->DR = tx_value;
    //while (!(SPI1->SR & SPI_SR_TXE_Msk)) ;
    
    while (!(SPI1->SR & SPI_SR_RXNE_Msk)) ;
    *rx_buffer = SPI1->DR;
    for (int i = 0; i < 10; i++) ;

    return SPI_OK;
}