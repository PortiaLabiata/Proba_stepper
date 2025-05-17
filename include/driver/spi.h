#pragma once
#include <stm32f103x6.h>
#include "core/system.h"
#include "core/types.h"

/* Macros */

#define SPI_START_OP() GPIOA->BSRR |= GPIO_BSRR_BR4;
#define SPI_STOP_OP() GPIOA->BSRR |= GPIO_BSRR_BS4;

/* Prototypes */

SPI_Status_t SPI_SendData(uint8_t *data, uint32_t size);
SPI_Status_t SPI_ReadData(uint8_t *data, uint32_t size);
SPI_Status_t SPI_Transfer(uint8_t tx_value, uint8_t *rx_buffer);