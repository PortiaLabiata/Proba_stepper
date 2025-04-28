#include "main.h"
#include "utils/service.h"
#include "driver/interrupts.h"
#include "driver/uart.h"
#include "driver/stepper.h"
#include "driver/iwdg.h"

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

#ifdef STARTUP_BLINK_ENABLE
    STARTUP_BLINK();
#endif

    IWDG_Config();

    Stepper_Handle_t *stp = Stepper_Init(TIM2, gpios, wave, NULL, NULL);
    UART_Handle_t *hnd = UART_Init(USART1);

    ctx.stepper_handle = stp;
    ctx.uart_handle = hnd;
    
    UART_Recieve(hnd, buffer, 3);
    UART_Transmit(hnd, (uint8_t*)"rdy\n", strlen("rdy\n"), MAX_TIMEOUT);
    Stepper_SetMode(ctx.stepper_handle, STEPPER_MODE_FULLSTEP_2PHASE);
    Stepper_Rotate_IT(ctx.stepper_handle, 10, CLOCKWISE, 10);

    while (1) {
        if (UART_GetCmdRdy(hnd)) {
            LOG("Command recieved\n");
            uint8_t response[1] = {ERR_RESP};
            if (ProcessCommand(stp, buffer, hnd) == SET) {
                response[0] = ACK_RESP;  
            }
            UART_Transmit(hnd, response, 1, MAX_TIMEOUT);
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
    int steps = *(cmd + 1);
    int speed = *(cmd + 2);
    switch (cmd[0]) {
        case FORW_CMD:
            Stepper_Rotate_IT(stp, steps, CLOCKWISE, speed);
            return SET;
        case REV_CMD:
            Stepper_Rotate_IT(stp, steps, COUNTERCLOCKWISE, speed);
            return SET;
        case HALT_CMD:
            Stepper_Halt_IT(stp, RESET);
            return SET;
        default:
            return RESET;
    }
}
