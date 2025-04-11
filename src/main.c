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

    hnd.instance = USART1;

    stp.config = wave;
    stp.gpios = gpios;
    UART_Recieve(&hnd, buffer, 3);
    Stepper_Init(&stp);
    //Stepper_Rotate_IT(&stp, 200, CLOCKWISE, 10);
    //Stepper_Halt(&stp, RESET);
    UART_Transmit(&hnd, (uint8_t*)"rdy\n", strlen("rdy\n"), MAX_TIMEOUT);

    while (1) {
        if (hnd.command_ready) {
            if (ProcessCommand(&stp, buffer, &hnd) != SET) {
                UART_Transmit(&hnd, (uint8_t*)"err\n", strlen("err\n"), MAX_TIMEOUT);
            }
        }
    }
}

/**
 * \todo Add error handling.
 */
uint8_t ProcessCommand(Stepper_Handle_t *stp, uint8_t *cmd, UART_Handle_t *handle) {
    handle->command_ready = RESET;
    UART_Recieve(handle, buffer, 3);
    int steps = atoi((char*)(cmd + 1)) * 50;
    int speed = atoi((char*)(cmd + 2)) * 10;
    switch (cmd[0]) {
        case 'f':
            Stepper_Rotate_IT(stp, steps, CLOCKWISE, speed);
            return SET;
        case 'r':
            Stepper_Rotate_IT(stp, steps, COUNTERCLOCKWISE, speed);
            return SET;
        case 'h':
            Stepper_Halt_IT(stp, RESET);
            return SET;
        default:
            return RESET;
    }
}
