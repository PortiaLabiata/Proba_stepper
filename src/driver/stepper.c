#include "driver/stepper.h"

/* Typedefs */

/**
 * \todo Microoptimization: switch from array to single variables. It will make time
 * of reading a value just a little bit shorter.
 */
struct Stepper_Handle {
    uint32_t *gpios;
    uint8_t *config;
    TIM_TypeDef *instance;
    volatile uint8_t config_idx;
    uint8_t direc;
    uint32_t steps_left;
};

/* Handle pool */

static Stepper_Handle_t _stepper_pool[MAX_STEPPERS];
static uint8_t _n_steppers;

/* Global definitions */

static Stepper_Handle_t *_stp;

/* Functions */

Stepper_Handle_t *Stepper_Init(uint32_t *gpios, uint8_t *configs) {
    Stepper_Handle_t *stp = &_stepper_pool[_n_steppers++];
    stp->gpios = gpios;
    stp->config = configs;
    stp->config_idx = 0;
    return stp;
}

/**
 * \brief Preforms a single step.
 * \param[in] stp Stepper handle.
 * \param[in] dir Direction, can be either CLOCKWISE or COUNTERCLOCKWISE.
 */
uint8_t Stepper_Step(Stepper_Handle_t *stp, uint8_t dir) {
    uint8_t curr_config = stp->config[stp->config_idx];
    GPIOB->ODR &= ~(stp->gpios[0] | stp->gpios[1] | stp->gpios[2] | stp->gpios[3]);

    if (curr_config & 0b0001) GPIOB->ODR |= stp->gpios[3];
    if (curr_config & 0b0010) GPIOB->ODR |= stp->gpios[2];
    if (curr_config & 0b0100) GPIOB->ODR |= stp->gpios[1];
    if (curr_config & 0b1000) GPIOB->ODR |= stp->gpios[0];

    if (dir == CLOCKWISE) {
        stp->config_idx = (stp->config_idx + 1) % N_PINS;
    } else if (dir == COUNTERCLOCKWISE) {
        stp->config_idx = (stp->config_idx - 1 + N_PINS) % N_PINS;
    } else {
        return RESET; // In case something went horribly wrong.
    }
    return SET;
}

uint8_t Stepper_Halt(Stepper_Handle_t *stp, uint8_t hold) {
    if (!hold) {
        GPIOB->ODR &= ~(stp->gpios[0] | stp->gpios[1] | stp->gpios[2] | stp->gpios[3]);
        return SET;
    } else {
        return SET;
    }
}


/**
 * \brief Performs a blocking stepper revolution. Currently only full step mode supported.
 * Minimum delay is 10 ms, technically there is no maximum delay, but at 100 ms the
 * current cunsomption increases to 300 mA, so I would say a max of 50-70 ms is recommended.
 * \param[in] stp Stepper handle.
 * \param[in] steps Number of steps performed.
 * \param[in] dir Direction, can be either CLOCKWISE or COUNTERCLOCKWISE.
 * \param[in] del Delay between steps.
 * \todo Add error handling.
 */
uint8_t Stepper_Rotate(Stepper_Handle_t *stp, uint32_t steps, uint8_t dir, uint32_t del) {
    for (int i = 0; i < steps; i++) {
        Stepper_Step(stp, dir);
        delay(del);
    }

    return SET;
}

uint8_t Stepper_Rotate_IT(Stepper_Handle_t *stp, uint32_t steps, uint8_t dir, uint32_t del) {
    stp->steps_left = steps;
    stp->direc = dir;
    _stp = stp;
    uint32_t cycles = PCLK1_FREQ * del / 1000;
    uint32_t psc = cycles / 65535 + 1;

    TIM3->PSC = psc;
    TIM3->ARR = cycles / psc - 1;
    TIM3->EGR |= TIM_EGR_UG;
    TIM3->CR1 |= TIM_CR1_CEN;
    return SET;
}

uint8_t Stepper_Halt_IT(Stepper_Handle_t *stp, uint8_t hold) {
    stp->steps_left = 0;
    return Stepper_Halt(stp, hold);
}

/* Callbacks */

uint8_t TIM_UEV_Callback(System_Context_t *ctx) {
    Stepper_Handle_t *handle = ctx->stepper_handle;
    if (handle->steps_left-- > 0) {
        Stepper_Step(handle, handle->direc);
        return SET;
    } else {
        Stepper_Halt(handle, RESET);
        return RESET;
    }
}

