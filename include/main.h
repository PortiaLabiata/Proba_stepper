#pragma once

#include <string.h>
#include "core/system.h"
#include "core/types.h"

/* Forward declarations */

typedef struct UART_Handle UART_Handle_t;
typedef struct Stepper_Handle Stepper_Handle_t;

/* UART configuration constants */

#define CMD_BUFFER_SIZE 2
#define STEPPER_DIVIDER 960

/* Global definitions */

extern UART_Handle_t *hnd;
extern volatile uint8_t buffer[10];

uint8_t ProcessCommand(Stepper_Handle_t *stp, uint8_t *cmd, UART_Handle_t *handle);
