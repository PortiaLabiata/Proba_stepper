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
    volatile uint8_t *cursor;
    volatile uint8_t error_code;
    volatile uint8_t command_ready;
    volatile uint32_t rx_left;
} UART_Handle_t; // Since I don't really need encapsulation, it's fine.

/* Global definitions */

extern UART_Handle_t handle;
