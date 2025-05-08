#include "main.h"
#include "utils/service.h"
#include "driver/interrupts.h"
#include "driver/uart.h"
#include "driver/stepper.h"
#include "driver/iwdg.h"

#include "mb.h"
#include "port.h"

#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4

static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];

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

    eMBErrorCode eStatus; // For later

    eStatus = eMBInit(MB_RTU, 1, 0, MB_BAUD_RATE, MB_PAR_EVEN);
    eStatus = eMBEnable();
    
    UART_Transmit(hnd, (uint8_t*)"rdy\n", strlen("rdy\n"), MAX_TIMEOUT);
    Stepper_SetMode(ctx.stepper_handle, STEPPER_MODE_FULLSTEP_2PHASE);
    Stepper_Rotate_IT(ctx.stepper_handle, 10, CLOCKWISE, 10);

    while (1) {
        (void)eMBPoll();

        IWDG_RELOAD();
    }

}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    return MB_ENOREG;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}
