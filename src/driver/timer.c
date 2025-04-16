#include "driver/timer.h"

/* Service functions */

static inline System_Status_t TIM_SetChannelMode(TIM_TypeDef *tim, uint8_t channel, uint32_t mode, uint16_t cc) {
    switch (channel) {
        case 1:
            TIM_SET_CH1MODE(tim, mode);
            tim->CCR1 = cc;
            TIM_CH1_ENABLE(tim);
            break;
        case 2:
            TIM_SET_CH2MODE(tim, mode);
            tim->CCR2 = cc;
            TIM_CH2_ENABLE(tim);
            break;
        case 3:
            TIM_SET_CH3MODE(tim, mode);
            tim->CCR3 = cc;
            TIM_CH3_ENABLE(tim);
            break;
        case 4:
            TIM_SET_CH4MODE(tim, mode);
            tim->CCR4 = cc;
            TIM_CH4_ENABLE(tim);
            break;
        default:
            return SYS_ERROR;
    }
    return SYS_OK;
}

/* Configuration functions */

System_Status_t TIM_SetCKD(TIM_TypeDef *tim, uint8_t ckd) {
    if (tim == NULL) return SYS_ERROR;
    tim->CR1 &= ~TIM_CR1_CKD_Msk;
    switch (ckd) {
        case TIM_CKD_1:
            break;
        case TIM_CKD_2:
            tim->CR1 |= TIM_CR1_CKD_0;
            break;
        case TIM_CKD_4:
            tim->CR1 |= TIM_CR1_CKD_1;
            break;
        default:
            return SYS_ERROR;
    }
    return SYS_OK;
}

System_Status_t TIM_Channel_Config(TIM_TypeDef *tim, uint8_t channel, \
    uint16_t cc, uint32_t mode) {
    if (tim == NULL) return SYS_ERROR;
    return TIM_SetChannelMode(tim, channel, mode, cc);
}

System_Status_t TIM_DelayChannel_Config(TIM_TypeDef *tim, uint8_t channel, \
    uint16_t cc, uint32_t mode) {
    if (tim == NULL) return SYS_ERROR;
    if (mode != TIM_CHMODE_ACTIVE && mode != TIM_CHMODE_INACTVE) return SYS_ERROR;
    return TIM_SetChannelMode(tim, channel, mode, cc);
}

/* Timer synchronisation */

System_Status_t TIM_SlaveConfigure(TIM_TypeDef *tim, TIM_SlaveTrigger_t source, \
        TIM_SlaveMode_t mode) {
    if (tim == NULL) return SYS_ERROR;
    if (source < TIM_SLAVE_TRIGGER_ITR0 || source > TIM_SLAVE_TRIGGER_ETRF) return SYS_ERROR;
    if (mode < TIM_SLAVE_MODE_DISABLE || mode > TIM_SLAVE_MODE_EXTCLOCK) return SYS_ERROR;

    tim->SMCR &= ~TIM_SMCR_TS_Msk;
    tim->SMCR |= ((uint32_t)source << TIM_SMCR_TS_Pos);
    tim->SMCR &= ~TIM_SMCR_SMS_Msk;
    tim->SMCR |= ((uint32_t)mode << TIM_SMCR_SMS_Pos);
    return SYS_OK;
}