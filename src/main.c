#include "main.h"
#include "utils/service.h"
#include "driver/interrupts.h"
#include "driver/uart.h"
#include "driver/stepper.h"
#include "driver/iwdg.h"

UART_Handle_t *hnd = NULL;
Stepper_Handle_t *stp = NULL;

const uint8_t wave[4] = {
    0b1000,
    0b0010,
    0b0100,
    0b0001
};

const uint8_t wave_full_phase[4] = {
    0b1010,
    0b0110,
    0b0101,
    0b1001
};

const uint32_t gpios[4] = {GPIO_ODR_ODR6, GPIO_ODR_ODR7, GPIO_ODR_ODR8, GPIO_ODR_ODR9};
volatile uint8_t buffer[10];

int main(void) {
    ClockConfig();
    GPIO_Config();
    UART_Config();
    TIM2_Config();

#ifdef USE_STARTUP_BLINK
    STARTUP_BLINK();
#endif

    IWDG_Config();

    stp = Stepper_Init(gpios, wave, NULL, NULL);
    hnd = UART_Init(USART1);

    ctx.stepper_handle = stp;
    ctx.uart_handle = hnd;
    
    UART_Recieve(hnd, buffer, 3);
    UART_Transmit(hnd, (uint8_t*)"rdy\n", strlen("rdy\n"), MAX_TIMEOUT);
    Stepper_SetMode(ctx.stepper_handle, STEPPER_MODE_FULLSTEP_2PHASE);
    Stepper_Rotate_IT(ctx.stepper_handle, 10, CLOCKWISE, 10);

    while (1) {
        if (UART_GetCmdRdy(hnd)) {
            if (ProcessCommand(stp, buffer, hnd) == SET) {
                UART_Transmit(hnd, (uint8_t*)"ack\n", strlen("ack\n"), MAX_TIMEOUT);
            } else {
                UART_Transmit(hnd, (uint8_t*)"err\n", strlen("err\n"), MAX_TIMEOUT);
            }
        }
        //delay(7000);
        IWDG_RELOAD();
    }

}

/**
 * \todo Add error handling.
 */
uint8_t ProcessCommand(Stepper_Handle_t *stp, volatile uint8_t *cmd, UART_Handle_t *handle) {
    UART_Recieve(handle, buffer, 3);
    UART_SetCmdRdy(handle, RESET);
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
