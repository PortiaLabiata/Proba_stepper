#pragma once
#include <stm32f103x6.h>
#include "core/system.h"
#include "core/types.h"

/* Prototypes */

SPI_Status_t SPI_SendData(uint8_t *data, uint32_t size);
SPI_Status_t SPI_ReadData(uint16_t *data, uint32_t size);