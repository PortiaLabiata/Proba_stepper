#include "core/types.h"
#include "core/system.h"

/* Context */

System_Context_t ctx;

/**
 * \brief Verbose initializaion of system context.
 * \param[out] ctx Pointer to system context.
 * \param[in] uart Pointer to initialized UART_Handle_t object.
 * \param[in] stp Pointer to initialized Stepper_Handle_t object.
 */
System_Status_t Context_Init(System_Context_t *ctx, UART_Handle_t *uart, \
    Stepper_Handle_t *stp) {
    if (uart == NULL || stp == NULL) {
        return SYS_ERROR_NULLPTR;
    }
    ctx->stepper_handle = stp;
    ctx->uart_handle = uart;
    return SYS_OK;
}

/* MB Proxy functions */


