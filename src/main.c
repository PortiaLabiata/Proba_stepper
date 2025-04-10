#include "main.h"
#include "configure.h"
#include "service.h"
#include "interrupts.h"
#include "uart.h"
#include "stepper.h"

UART_Handle_t hnd;
Stepper_Handle_t stp;

const uint8_t wave[4] = {
    0b1000,
    0b0010,
    0b0100,
    0b0001
};

const uint32_t gpios[4] = {GPIO_ODR_ODR6, GPIO_ODR_ODR7, GPIO_ODR_ODR8, GPIO_ODR_ODR9};
volatile uint8_t buffer[10];

int main(void) {
    ClockConfig();
    GPIO_Config();
    UART_Config();
    TIM3_Config();

    stp.config = wave;
    stp.gpios = gpios;
    UART_Recieve(&hnd, buffer, 2);
    Stepper_Init(&stp);
    //Stepper_Rotate_IT(&stp, 200, CLOCKWISE, 10);
    //Stepper_Halt(&stp, RESET);

    while (1) {
        if (hnd.command_ready) {
            if (ProcessCommand(&stp, buffer, &hnd) != SET) {
                UART_Transmit((uint8_t*)"er", strlen("er"), MAX_TIMEOUT);
            }
        }
    }
}

/**
 * \todo Add error handling.
 */
uint8_t ProcessCommand(Stepper_Handle_t *stp, uint8_t *cmd, UART_Handle_t *handle) {
    handle->command_ready = RESET;
    UART_Recieve(handle, buffer, 2);
    uint32_t steps = atoi((char*)(cmd + 1)) * 50;
    switch (cmd[0]) {
        case 'f':
            Stepper_Rotate_IT(stp, steps, CLOCKWISE, 10);
            return SET;
        case 'r':
            Stepper_Rotate_IT(stp, steps, COUNTERCLOCKWISE, 10);
            return SET;
        default:
            return RESET;
    }
}
