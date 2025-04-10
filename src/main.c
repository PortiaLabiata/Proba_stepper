#include "main.h"
#include "configure.h"
#include "service.h"
#include "interrupts.h"
#include "uart.h"
#include "stepper.h"

UART_Handle_t handle;
Stepper_t stp;

const uint8_t wave[4] = {
    0b1000,
    0b0010,
    0b0100,
    0b0001
};

const uint32_t gpios[4] = {GPIO_ODR_ODR6, GPIO_ODR_ODR7, GPIO_ODR_ODR8, GPIO_ODR_ODR9};

int main(void) {
    ClockConfig();
    GPIO_Config();
    UART_Config();

    stp.config = wave;
    stp.gpios = gpios;
    Stepper_Init(&stp);

    while (1) {
        Stepper_Rotate(&stp, 50, CLOCKWISE, 10);
        delay(100);
        Stepper_Rotate(&stp, 50, COUNTERCLOCKWISE, 10);
        delay(100);
    }
}
