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
#include "uip_arp.h"

#include "mb.h"
#include "port.h"

//static System_Status_t eSystemPoll(MB_Proxy_t *proxy); 
#define BUF ((struct uip_eth_hdr *)uip_buf)
volatile uint8_t ppend = RESET;

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
    eMBTCPInit(MB_PORT);
    eMBEnable();

    MB_Proxy_t *proxy = MB_Proxy_Init();
    Context_Init(&ctx, hnd, stp);
    net_init();

    while (1) {
        eMBPoll();
        if (!ETH_INT_STATE()) {
            uip_len = ENC_RecievePacket(uip_buf);
        }
        if (uip_len > 0) {
            if (BUF->type == HTONS(UIP_ETHTYPE_IP)) {
                uip_arp_ipin();
                uip_input();

                if (uip_len > 0) {
                    uip_arp_out();
                    dev_send();
                }

            } else if (BUF->type == HTONS(UIP_ETHTYPE_ARP)) {
                uip_arp_arpin();
                if (uip_len > 0) {
                    dev_send();
                }

            }
        }
        if (timer_expired(&periodic_timer)) {
            for(int i = 0; i < UIP_CONNS; i++) {
                uip_periodic(i);
                if(uip_len > 0) {
                  uip_arp_out();
                  dev_send();
                }
            }
        }

        if(timer_expired(&arp_timer)) {
            timer_reset(&arp_timer);
            uip_arp_timer();
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
