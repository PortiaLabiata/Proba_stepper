#include "main.h"
#include "utils/service.h"
#include "driver/interrupts.h"
#include "driver/uart.h"
#include "driver/stepper.h"
#include "driver/iwdg.h"

#include "mb.h"
#include "port.h"

//#include "app/modbus.h"

#define REG_INPUT_START     1
#define REG_INPUT_NREGS     1
#define REG_HOLDING_START   1
#define REG_HOLDING_NREGS   4
#define COIL_START          1
#define COIL_NCOILS         1
#define DISCR_START         1
#define DISCR_NDISCR        1

static USHORT usRegInputBuf[REG_INPUT_NREGS];
static USHORT usRegHoldingBuf[REG_HOLDING_NREGS];
static USHORT usCoilBuf[COIL_NCOILS];
static USHORT usDiscrBuf[DISCR_NDISCR];

static System_Status_t eSystemPoll(void);

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
    
    Stepper_SetMode(ctx.stepper_handle, STEPPER_MODE_FULLSTEP_2PHASE);
    Stepper_Rotate_IT(ctx.stepper_handle, 10, CLOCKWISE, 10);

    while (1) {
        (void)eMBPoll();
        (void)eSystemPoll();
        IWDG_RELOAD();
    }

}

/**
 * \todo Add error handling.
 */
static System_Status_t eSystemPoll(void) {
    switch ((Command_t)usRegHoldingBuf[INDEX_HOLD_CMD]) {
        case CMD_NOCMD:
            return SYS_OK;
        case CMD_ROTATE:
            Stepper_Rotate_IT(ctx.stepper_handle, usRegHoldingBuf[INDEX_HOLD_STEPS], \
             usRegHoldingBuf[INDEX_HOLD_MODE] & 0xFF, usRegHoldingBuf[INDEX_HOLD_SPEED]);
            break;
        case CMD_HALT:
            Stepper_Halt_IT(ctx.stepper_handle, RESET);
            break;
    }
    usRegHoldingBuf[INDEX_HOLD_CMD] = 0;
    return SYS_OK;
}

static eMBErrorCode eMBGenericRead(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, \
     USHORT usStart, USHORT usNumber, USHORT *usDataBuffer) {
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex = 0;

    if( ( usAddress >= usStart )
        && ( usAddress + usNRegs <= usStart + usNumber ) )
    {
        iRegIndex = ( int )( usAddress - usStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usDataBuffer[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usDataBuffer[iRegIndex] & 0xFF );
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

static eMBErrorCode eMBGenericWriteReg(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, \
    USHORT usStart, USHORT usNumber, USHORT *usDataBuffer) {
    eMBErrorCode eStatus = MB_ENOERR;

    if ((usAddress >= usStart) && (usAddress + usNRegs <= usStart + usNumber)) {
        int iRegIndex = (int)(usAddress - usStart);
        for (USHORT iIndex = 0; iRegIndex < usAddress - usStart + usNRegs; ) {
            USHORT usRegValue = pucRegBuffer[iIndex++] << 8;
            usRegValue += pucRegBuffer[iIndex++] & 0xFF;
            usDataBuffer[iRegIndex++] = usRegValue;
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode eStatus = eMBGenericRead(pucRegBuffer, usAddress, usNRegs, \
        REG_INPUT_START, REG_INPUT_NREGS, usRegInputBuf);
    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    /* First we have to make sure, that we reply */
    eMBErrorCode eStatus = MB_ENOERR;
    if (eMode == MB_REG_READ) {
        eStatus = eMBGenericRead(pucRegBuffer, usAddress, usNRegs, \
            REG_HOLDING_START, REG_HOLDING_NREGS, usRegHoldingBuf);
    } else if (eMode == MB_REG_WRITE) {
        eStatus = eMBGenericWriteReg(pucRegBuffer, usAddress, usNRegs, \
            REG_HOLDING_START, REG_HOLDING_NREGS, usRegHoldingBuf);
    }
    return eStatus;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    eMBErrorCode eStatus = MB_ENOERR;
    if (eMode == MB_REG_READ) {
        eStatus = eMBGenericRead(pucRegBuffer, usAddress, usNCoils, \
            COIL_START, COIL_NCOILS, usCoilBuf);
    } else if (eMode == MB_REG_WRITE) {
        eStatus = eMBGenericWriteReg(pucRegBuffer, usAddress, usNCoils, \
            COIL_START, COIL_NCOILS, usCoilBuf);
    }
    return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode eStatus = eMBGenericRead(pucRegBuffer, usAddress, usNDiscrete, \
        DISCR_START, DISCR_NDISCR, usDiscrBuf);
    return eStatus;
}
