//#include <stdlib.h>

#include "main.h"
#include "service.h"

/* Constants */

#define CLOCKWISE (uint8_t)0
#define COUNTERCLOCKWISE (uint8_t)1
#define N_PINS (uint8_t)4

/* Functions */

uint8_t Stepper_Init(Stepper_Handle_t *stp);
uint8_t Stepper_Step(Stepper_Handle_t *stp, uint8_t dir);
uint8_t Stepper_Halt(Stepper_Handle_t *stp, uint8_t hold);
uint8_t Stepper_Rotate(Stepper_Handle_t *stp, uint32_t steps, uint8_t dir, uint32_t del);
uint8_t Stepper_Rotate_IT(Stepper_Handle_t *stp, uint32_t steps, uint8_t dir, uint32_t del);

/* ISRs */

void TIM3_IRQHandler(void);
