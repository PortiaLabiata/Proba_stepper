#pragma once

#include <string.h>
#include <stm32f103x6.h>

/* Global flag options */

#define SET (uint8_t)1
#define RESET (uint8_t)0

/* UART configuration constants */

#define UART_BAUD_RATE 115200
#define CMD_BUFFER_SIZE 2
#define MAX_TIMEOUT -1
#define STEPPER_DIVIDER 960

/* Typedefs */

typedef struct {
    USART_TypeDef *instance;
    volatile uint8_t *cursor;
    volatile uint8_t error_code;
    volatile uint8_t command_ready;
    volatile uint32_t rx_left;
} UART_Handle_t; // Since I don't really need encapsulation, it's fine.

/**
 * \struct
 * \brief Stepper structure. GPIOS should be in form of GPIO_ODR_ODRx_Msk.
 */
typedef struct {
    uint32_t *gpios;
    uint8_t *config;
    volatile uint8_t config_idx;
    uint8_t direc;
    uint32_t steps_left;
} Stepper_Handle_t;

/* Global definitions */

extern UART_Handle_t hnd;
extern volatile uint8_t buffer[10];

uint8_t ProcessCommand(Stepper_Handle_t *stp, uint8_t *cmd, UART_Handle_t *handle);
