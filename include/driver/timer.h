#include "main.h"

#ifndef TIM8
    #define TIM8 NULL
#endif

/* Enable clocking */

#define TIM1_CLOCKING_ENABLE() RCC->APB2ENR |= RCC_APB2ENR_TIM1EN
#define TIM2_CLOCKING_ENABLE() RCC->APB1ENR |= RCC_APB1ENR_TIM2EN
#define TIM3_CLOCKING_ENABLE() RCC->APB1ENR |= RCC_APB1ENR_TIM3EN

#define TIM1_CLOCKING_DISABLE() RCC->APB2ENR &= ~RCC_APB2ENR_TIM1EN_Msk
#define TIM2_CLOCKING_DISABLE() RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN_Msk
#define TIM3_CLOCKING_DISABLE() RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN_Msk

/* Set direction */
#define TIM_DIR_UP(__TIM__) __TIM__->CR1 &= ~TIM_CR1_DIR_Msk;
#define TIM_DIR_DOWN(__TIM__) __TIM__->CR1 |= TIM_CR1_DIR;

/* Enable */
#define TIM_ENABLE(__TIM__) __TIM__->CR1 |= TIM_CR1_CEN;
#define TIM_DISABLE(__TIM__) __TIM__->CR1 &= ~TIM_CR1_CEN;

/* Channels config */
#define TIM_CHMODE_FROZEN      0b000UL
#define TIM_CHMODE_ACTIVE      0b001UL
#define TIM_CHMODE_INACTVE     0b010UL
#define TIM_CHMODE_TOGGLE      0b011UL
#define TIM_CHMODE_FINACTIVE   0b100UL
#define TIM_CHMODE_FACTIVE     0b101UL
#define TIM_CHMODE_PWM1        0b110UL
#define TIM_CHMODE_PWM2        0b111UL

// Cannot be used with variables

#define TIM_SET_CH12MODE(__TIM__, __CHANNEL__, __MODE__) do { \
    __TIM__->CCMR1 &= ~TIM_CCMR1_OC##__CHANNEL__##M_Msk; \
    __TIM__->CCMR1 |= (__MODE__ << TIM_CCMR1_OC##__CHANNEL__##M_Pos); \
} while (0)

#define TIM_SET_CH34MODE(__TIM__, __CHANNEL__, __MODE__) do { \
    __TIM__->CCMR2 &= ~TIM_CCMR2_OC##__CHANNEL__##M_Msk; \
    __TIM__->CCMR2 |= (__MODE__ << TIM_CCMR2_OC##__CHANNEL__##M_Pos); \
} while (0)

// Can be used with variables

#define TIM_SET_CH1MODE(__TIM__, __MODE__) do { \
    __TIM__->CCMR1 &= ~TIM_CCMR1_OC1M_Msk; \
    __TIM__->CCMR1 |= (__MODE__ << TIM_CCMR1_OC1M_Pos); \
} while (0)

#define TIM_SET_CH2MODE(__TIM__, __MODE__) do { \
    __TIM__->CCMR1 &= ~TIM_CCMR1_OC2M_Msk; \
    __TIM__->CCMR1 |= (__MODE__ << TIM_CCMR1_OC2M_Pos); \
} while (0)

#define TIM_SET_CH3MODE(__TIM__, __MODE__) do { \
    __TIM__->CCMR2 &= ~TIM_CCMR2_OC3M_Msk; \
    __TIM__->CCMR2 |= (__MODE__ << TIM_CCMR2_OC3M_Pos); \
} while (0)

#define TIM_SET_CH4MODE(__TIM__, __MODE__) do { \
    __TIM__->CCMR2 &= ~TIM_CCMR2_OC4M_Msk; \
    __TIM__->CCMR2 |= (__MODE__ << TIM_CCMR2_OC4M_Pos); \
} while (0)

// Will not work with variables

#define TIM_CH_ENABLE(__TIM__, __CHANNEL__) __TIM__->CCER |= TIM_CCER_CC##__CHANNEL__##E
#define TIM_CH_DISABLE(__TIM__, __CHANNEL__) __TIM__->CCER &= ~TIM_CCER_CC##__CHANNEL__##E_Msk

// Will work with variables

#define TIM_CH1_ENABLE(__TIM__) __TIM__->CCER |= TIM_CCER_CC1E;
#define TIM_CH2_ENABLE(__TIM__) __TIM__->CCER |= TIM_CCER_CC2E;
#define TIM_CH3_ENABLE(__TIM__) __TIM__->CCER |= TIM_CCER_CC3E;
#define TIM_CH4_ENABLE(__TIM__) __TIM__->CCER |= TIM_CCER_CC4E;

#define TIM_CH1_DISABLE(__TIM__) __TIM__->CCER &= ~TIM_CCER_CC1E_Msk
#define TIM_CH2_DISABLE(__TIM__) __TIM__->CCER &= ~TIM_CCER_CC2E_Msk
#define TIM_CH3_DISABLE(__TIM__) __TIM__->CCER &= ~TIM_CCER_CC3E_Msk
#define TIM_CH4_DISABLE(__TIM__) __TIM__->CCER &= ~TIM_CCER_CC4E_Msk

/* Channels in advanced timers */

#define TIM_CHN_ENABLE(__TIM__, __CHANNEL__) __TIM__->CCER |= TIM_CCER_CC##__CHANNEL__##NE
#define TIM_CHN_DISABLE(__TIM__, __CHANNEL__) __TIM__->CCER &= ~TIM_CCER_CC##__CHANNEL__##NE

/* Events */

#define TIM_CLEAR_UIF(__TIM__) __TIM__->SR &= ~TIM_SR_UIF
#define TIM_UPDATE(__TIM__) __TIM__->EGR |= TIM_EGR_UG
#define TIM_UPDATE_INT_ENABLE(__TIM__) __TIM__->DIER |= TIM_DIER_UIE
#define TIM_UPDATE_INT_DISABLE(__TIM__) __TIM__->DIER &= ~TIM_DIER_UIE

/* Service */

#define TIM_ADV_MOE_SET(__TIM__) do { \
    if ((__TIM__ == TIM1) || (__TIM__ == TIM8)) { \
        __TIM__->BDTR |= TIM_BDTR_MOE; \
    } \
} while (0)
#define TIM_ADV_MOE_RESET(__TIM__) __TIM__->BDTR &= ~TIM_BDTR_MOE_Msk;

#define TIM_CKD_1   0
#define TIM_CKD_2   1
#define TIM_CKD_4   2

/* Synchronization */

typedef enum {
    TIM_MMS_RESET,
    TIM_MMS_ENABLE,
    TIM_MMS_UPDATE,
    TIM_MMS_COMPUL,
    TIM_MMS_OC1REF,
    TIM_MMS_OC2REF,
    TIM_MMS_OC3REF,
    TIM_MMS_OC4REF
} TIM_MasterMode_t;

#define TIM_MASTERMODE_SET(__TIM__, __MODE__) do { \
    __TIM__->CR2 |= (__MODE__ << TIM_CR2_MMS_Pos); \
    __TIM__->SMCR |= TIM_SMCR_MSM; \
} while (0)

typedef enum {
    TIM_SLAVE_TRIGGER_ITR0,
    TIM_SLAVE_TRIGGER_ITR1,
    TIM_SLAVE_TRIGGER_ITR2,
    TIM_SLAVE_TRIGGER_ITR3,
    TIM_SLAVE_TRIGGER_T1ED,
    TIM_SLAVE_TRIGGER_T1FP,
    TIM_SLAVE_TRIGGER_T2FP,
    TIM_SLAVE_TRIGGER_ETRF
} TIM_SlaveTrigger_t;

typedef enum {
    TIM_SLAVE_MODE_DISABLE,
    TIM_SLAVE_MODE_ENCODER1,
    TIM_SLAVE_MODE_ENCODER2,
    TIM_SLAVE_MODE_ENCODER3,
    TIM_SLAVE_MODE_RESET,
    TIM_SLAVE_MODE_GATED,
    TIM_SLAVE_MODE_TRIGGER,
    TIM_SLAVE_MODE_EXTCLOCK
} TIM_SlaveMode_t;

/* Prototypes */

System_Status_t TIM_SetCKD(TIM_TypeDef *tim, uint8_t ckd);
System_Status_t TIM_SlaveConfigure(TIM_TypeDef *tim, TIM_SlaveTrigger_t source, \
    TIM_SlaveMode_t mode);

System_Status_t TIM_Channel_Config(TIM_TypeDef *tim, uint8_t channel, \
    uint16_t cc, uint32_t mode);

System_Status_t TIM_DelayChannel_Config(TIM_TypeDef *tim, uint8_t channel, \
    uint16_t cc, uint32_t mode);
