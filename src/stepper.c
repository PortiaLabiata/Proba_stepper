#include "stepper.h"

/* Functions */

uint8_t Stepper_Init(Stepper_t *stp) {
    stp->config_idx = 0;
    return SET;
}

/**
 * \brief Preforms a single step.
 * \param[in] stp Stepper handle.
 * \param[in] dir Direction, can be either CLOCKWISE or COUNTERCLOCKWISE.
 */
uint8_t Stepper_Step(Stepper_t *stp, uint8_t dir) {
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

uint8_t Stepper_Halt(Stepper_t *stp, uint8_t hold) {
    if (!hold) {
        GPIOB->ODR &= ~(stp->gpios[0] | stp->gpios[1] | stp->gpios[2] | stp->gpios[3]);
        return SET;
    } else {
        return SET;
    }
}


/**
 * \brief Performs a blocking stepper revolution. Currently only full step mode supported.
 * \param[in] stp Stepper handle.
 * \param[in] steps Number of steps performed.
 * \param[in] dir Direction, can be either CLOCKWISE or COUNTERCLOCKWISE.
 * \param[in] del Delay between steps.
 * \todo Add error handling.
 */
uint8_t Stepper_Rotate(Stepper_t *stp, uint32_t steps, uint8_t dir, uint32_t del) {
    for (int i = 0; i < steps; i++) {
        Stepper_Step(stp, dir);
        delay(del);
    }

    return SET;
}