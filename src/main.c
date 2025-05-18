#include "main.h"
#include "utils/service.h"
#include "driver/interrupts.h"
#include "driver/uart.h"
#include "driver/stepper.h"
#include "driver/iwdg.h"
#include "driver/spi.h"
#include "driver/enc28j60.h"

#include "app/mb_logic.h"
#include "app/net.h"

#include "mb.h"
#include "port.h"

//static System_Status_t eSystemPoll(MB_Proxy_t *proxy); 

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
    Clock_Config();
    GPIO_Config();
    TIM2_Config();
    TIM3_Config();
    SPI_Config();
    
#if STARTUP_BLINK_ENABLE==1
    STARTUP_BLINK();
#endif

    IWDG_Config();

    Stepper_Handle_t *stp = Stepper_Init(TIM2, gpios, wave, NULL, NULL);
    UART_Handle_t *hnd = UART_Init(USART1);
    //MB_Proxy_t *proxy = MB_Proxy_Init();
    Context_Init(&ctx, hnd, stp);
    net_init();

    uint8_t rx_buff[1514];
    uint8_t dst_mac[6] = {0x00, 0xe0, 0x99, 0x00, 0x09, 0x85};
    uint8_t type_len[2] = {0x00, 0x04};
    uint32_t size = 0;
    
    delay(2);
    while (1) {
        size = ENC_RecievePacket(rx_buff);
        if (size > 0) {
            type_len[0] = size >> 8;
            type_len[1] = size & 0xFF;
            ENC_SendPacket(dst_mac, type_len, rx_buff + ETH_HEAD_SIZE - 1, size);
            size = 0;
        }
        IWDG_RELOAD();
    }
}

/**
 * \todo Add error handling.
 */
/* static System_Status_t eSystemPoll(MB_Proxy_t *proxy) {
    MB_Proxy_Unmarshall(proxy);
    System_Status_t status = SYS_OK;
    uint32_t speed = 0;
    switch (MB_Proxy_AccessCmd(proxy)) {
        case CMD_NOCMD:
            break;

        case CMD_ROTATE:
            speed = MB_Proxy_AccessSpeed(proxy);
            if (speed < 10 || speed > 50) {
                MB_Proxy_SetCmd(proxy, CMD_NOCMD);
                status = SYS_ERROR_ILLVALUE;
                break;
            }

            Stepper_SetMode(ctx.stepper_handle, MB_Proxy_AccessMode(proxy));
            Stepper_Rotate_IT(ctx.stepper_handle, MB_Proxy_AccessSteps(proxy), \
                MB_Proxy_AccessDir(proxy), MB_Proxy_AccessSpeed(proxy));
            break;

        case CMD_HALT:
            Stepper_Halt_IT(ctx.stepper_handle, RESET);
            break;
        
        case CMD_INVALID:
            status = SYS_ERROR_ILLVALUE;
            break;
        
        default:
            status = SYS_ERROR;
            break;
    }
    MB_Proxy_SetCmd(proxy, CMD_NOCMD);
    MB_Proxy_Marshall(proxy);
    return status;
}  */
