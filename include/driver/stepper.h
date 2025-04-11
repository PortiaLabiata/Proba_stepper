#include "core/system.h"
#include "core/types.h"

/* Constants */

#define CLOCKWISE (uint8_t)0
#define COUNTERCLOCKWISE (uint8_t)1
#define N_PINS (uint8_t)4
#define MAX_STEPPERS 2

/* Typedefs */

typedef struct Stepper_Handle Stepper_Handle_t;

/* Functions */

Stepper_Handle_t *Stepper_Init(uint32_t *gpios, uint8_t *configs);
Stepper_Status_t Stepper_Step(Stepper_Handle_t *stp, uint8_t dir);

Stepper_Status_t Stepper_Halt(Stepper_Handle_t *stp, uint8_t hold);
Stepper_Status_t Stepper_Rotate(Stepper_Handle_t *stp, uint32_t steps, uint8_t dir, uint32_t del);
Stepper_Status_t Stepper_Rotate_IT(Stepper_Handle_t *stp, uint32_t steps, uint8_t dir, uint32_t del);
Stepper_Status_t Stepper_Halt_IT(Stepper_Handle_t *stp, uint8_t hold);

/* ISRs */

uint8_t TIM_UEV_Callback(System_Context_t *ctx);
