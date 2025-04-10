#include "main.h"
#include "configure.h"
#include "service.h"
#include "interrupts.h"
#include "uart.h"
#include "stepper.h"

UART_Handle_t handle;
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
    UART_Recieve(buffer, 2);
    Stepper_Init(&stp);
    //Stepper_Rotate_IT(&stp, 200, CLOCKWISE, 10);
    //Stepper_Halt(&stp, RESET);

    while (1) {
        if (handle.command_ready) {
            if (ProcessCommand(&stp, buffer) != SET) {
                UART_Transmit((uint8_t*)"er", strlen("er"), MAX_TIMEOUT);
            }
        }
    }
}
