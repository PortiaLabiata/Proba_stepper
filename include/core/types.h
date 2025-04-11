#pragma once

/* Global flag options */

#define SET (uint8_t)1
#define RESET (uint8_t)0
#define MAX_TIMEOUT -1

/* Forward declarations */

typedef struct UART_Handle UART_Handle_t;
typedef struct Stepper_Handle Stepper_Handle_t;

/* Global typedefs */

typedef struct {
    UART_Handle_t *uart_handle;
    Stepper_Handle_t *stepper_handle;
} System_Context_t;

typedef enum {
    STEPPER_OK,
    STEPPER_ERROR_SOFT
} Stepper_Status_t;

typedef enum {
    UART_OK,
    UART_ERROR_SOFT
} UART_Status_t;

/* System context */

extern System_Context_t ctx;