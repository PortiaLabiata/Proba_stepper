#include "driver/uart.h"

/* Typedefs */

struct UART_Handle {
    volatile USART_TypeDef *instance;
    volatile uint8_t *cursor;
    volatile uint8_t command_ready;
    volatile uint32_t rx_left;
    volatile uint32_t tx_left;
    volatile uint8_t *cursor_tx;
};

/* Handle pool */

static UART_Handle_t _uart_pools[MAX_UARTS];
static uint8_t _n_uarts = 0;

/* Configuration */

/**
 * \brief Initializes UART handle object from the global pool.
 * \param[in] inst UART_TypeDef instance.
 * \returns Pointer to the initialized object.
 */
UART_Handle_t *UART_Init(USART_TypeDef *inst) {
    if (_n_uarts >= MAX_UARTS) return NULL;

    UART_Handle_t *handle = &_uart_pools[_n_uarts++];
    handle->rx_left = 0;
    handle->command_ready = RESET;
    handle->instance = inst;

    USART1->CR1 |= (USART_CR1_RXNEIE | USART_CR1_TXEIE); // Enable interrupts
    return handle;
}

/* IO */

/**
 * \brief Transmits data through a UART. Blocking!
 * \param[in] handle UART handle object.
 * \param[in] pData Transferred data.
 * \param[in] size Data size.
 * \param[in] timeout Timeout in ticks, if equale to MAX_DELAY, will wait forever.
 * \returns Operation status.
 */
UART_Status_t UART_Transmit(UART_Handle_t *handle, uint8_t *pData, uint32_t size, int timeout) {
    uint32_t ms_start = Get_CurrentTick();

    for (uint8_t *data = pData; data - pData < size; data++) {
        while (!(handle->instance->SR & USART_SR_TXE_Msk)) {
            if (timeout != MAX_TIMEOUT && Get_CurrentTick() - ms_start > timeout) {
                return UART_ERROR_SOFT;
            }
        } // Wait until TXE bit is set.
        handle->instance->DR = *data; // Transmit
    }
    while (!(handle->instance->SR & USART_SR_TC_Msk)) {} // Wait until TC flag is set
    return UART_OK;
}

/**
 * \brief Starts writing the data into the buffer, non-blocking.
 * \param[in] handle UART handle.
 * \param[out] pData Buffer.
 * \param[in] size Data size.
 * \returns Operation status.
 */
UART_Status_t UART_Recieve(UART_Handle_t *handle, volatile uint8_t *pData, uint32_t size) {
    handle->rx_left = size;
    handle->cursor = pData;
    return UART_OK;
}

UART_Status_t UART_Transmit_IT(UART_Handle_t *handle, volatile uint8_t *pData, uint32_t size) {
    while (!(handle->instance->SR & USART_SR_TXE_Msk)) {
        __NOP();
    }
    handle->cursor_tx = pData;
    handle->tx_left = size - 1;
    handle->instance->DR = *pData;
    return UART_OK;
}

/* Getters/setters */

uint8_t UART_GetCmdRdy(UART_Handle_t *handle) {
    return handle->command_ready;
}

uint8_t UART_GetRxLeft(UART_Handle_t *handle) {
    return handle->rx_left;
}

void UART_SetCmdRdy(UART_Handle_t *handle, uint8_t status) {
    handle->command_ready = status;
}

void UART_SetRxLeft(UART_Handle_t *handle, uint8_t value) {
    handle->rx_left = value;
}

/* Callbacks */

/**
 * \brief UART RX callback.
 * \param[in] ctx System context.
 * \returns Operation status, either SET or RESET, likewise to TIM callback.
 */
uint8_t UART_RecieveCallback(System_Context_t *ctx) {
    UART_Handle_t *handle = ctx->uart_handle;
     // Character recieved
    *handle->cursor = handle->instance->DR;
    handle->cursor++;
    handle->rx_left--;

    if (handle->rx_left == 0) {
        handle->command_ready = SET;
    }
   return SET;
}

UART_Status_t UART_TransmitCallback(UART_Handle_t *handle) {
    if (handle->tx_left-- > 0) {
        handle->instance->DR = *(handle->cursor_tx++);
    }
}
