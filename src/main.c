#include "main.h"
#include "utils/service.h"
#include "driver/interrupts.h"
#include "driver/uart.h"
#include "driver/stepper.h"
#include "driver/timer.h"

UART_Handle_t *hnd = NULL;
Stepper_Handle_t *stp = NULL;

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
    
    TIM1_CLOCKING_ENABLE();
    TIM_SetCKD(TIM1, TIM_CKD_4);
    TIM_DIR_UP(TIM1);
    TIM_UPDATE_INT_DISABLE(TIM1);

    TIM_MASTERMODE_SET(TIM1, TIM_MMS_OC3REF);

    TIM1->PSC = 4799;
    TIM1->ARR = 199;

    TIM_Channel_Config(TIM1, 1, 99, TIM_CHMODE_PWM1);
    TIM_Channel_Config(TIM1, 2, 105, TIM_CHMODE_PWM2);
    TIM_DelayChannel_Config(TIM1, 3, 55, TIM_CHMODE_ACTIVE);
    TIM_UPDATE(TIM1);
    
    TIM_ADV_MOE_SET(TIM1);

    TIM2_CLOCKING_ENABLE();
    TIM_SetCKD(TIM2, TIM_CKD_4);
    TIM_DIR_UP(TIM2);
    TIM_UPDATE_INT_DISABLE(TIM2);

    TIM2->PSC = 4799;
    TIM2->ARR = 199;

    TIM_Channel_Config(TIM2, 1, 99, TIM_CHMODE_PWM1);
    TIM_Channel_Config(TIM2, 2, 105, TIM_CHMODE_PWM2);
    TIM_SlaveConfigure(TIM2, TIM_SLAVE_TRIGGER_ITR0, TIM_SLAVE_MODE_TRIGGER);
    TIM_UPDATE(TIM2);
    TIM2->CNT = 299;

    TIM_ENABLE(TIM1);

    while (1) ;

}

/**
 * \todo Add error handling.
 */
uint8_t ProcessCommand(Stepper_Handle_t *stp, uint8_t *cmd, UART_Handle_t *handle) {
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
