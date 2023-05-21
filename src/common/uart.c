#include "uart.h"
#include "hal_stm32.h"
#include "debug.h"

/**
 * @brief Initiate UART object
 *
 * @param uart UART object
 * @param autostart Register Callbacks and Start Receiving.
 */
void uart_init(uart_t *uart, bool autostart)
{
    uart->internal_init();
    queue_init(&uart->queue_rx, uart->queue_buffer_rx, uart->queue_buffer_rx_size);
    if(autostart) {
        uart_abort(uart);
        uart_register_internal_callbacks(uart);
        uart_rx_continue(uart);
    }
}


/**
 * @brief Register TX Completion callback (called on error and txcplt)
 *
 * @param uart UART object
 * @param txcompletion_cb Point to callback
 */
void uart_register_txcompletion(uart_t *uart, uart_txcompletion_func_t txcompletion_cb)
{
    if(uart) uart->cb_txcomplete = txcompletion_cb;
}

/**
 * @brief Register internal callbacks for UART object
 *
 * @param uart UART object
 */
void uart_register_internal_callbacks(uart_t *uart)
{
#if USE_HAL_UART_REGISTER_CALLBACKS == 1
    HAL_UART_RegisterCallback(uart->huart, HAL_UART_ERROR_CB_ID, uart->callback_error);
    HAL_UART_RegisterCallback(uart->huart, HAL_UART_TX_COMPLETE_CB_ID, uart->callback_txcplt);
    HAL_UART_RegisterCallback(uart->huart, HAL_UART_RX_COMPLETE_CB_ID, uart->callback_rxcplt);      // IT Only
    HAL_UART_RegisterRxEventCallback(uart->huart, uart->callback_rx_event);                         // ReceiveToIdle (DMA/IT)
#else
#error USE_HAL_UART_REGISTER_CALLBACKS must be 1
#endif
}

/**
 * @brief Start/Continue UART Transfer for RX
 *
 * @param uart UART object
 */
void uart_rx_continue(uart_t *uart)
{
    HAL_StatusTypeDef r;
    if(uart->hdma_rx) {
        // Receive an amount of data in DMA mode till either the expected number of data is received or an IDLE event occurs
        // Note that IDLE is roughly 1 byte length
        r = HAL_UARTEx_ReceiveToIdle_DMA(uart->huart, (uint8_t *)uart->data_buffer_rx, uart->data_buffer_rx_size);
        __HAL_DMA_DISABLE_IT(uart->hdma_rx, DMA_IT_HT);     // Disable half complete interrupt
    }
    else {
        r = HAL_UARTEx_ReceiveToIdle_IT(uart->huart, (uint8_t *) uart->data_buffer_rx, uart->data_buffer_rx_size);
    }
    if(r != HAL_OK) SR_DEBUG("uart_rx_continue failed");
}

/**
 * @brief Abort RX Transfer
 *
 * @param uart UART object
 */
void uart_rx_abort(uart_t *uart)
{
    HAL_UART_AbortReceive_IT(uart->huart);
}

/**
 * @brief Abort ALL transfers
 *
 * @param uart UART object
 */
void uart_abort(uart_t *uart)
{
    HAL_UART_Abort(uart->huart);
}

/**
 * @brief Internal RX handler
 */
static void uart_rx_internal(uart_t *uart, uint16_t size)
{
    if(uart->huart->Init.WordLength == UART_WORDLENGTH_9B) size *= 2;

    int written = queue_push(&uart->queue_rx, uart->data_buffer_rx, size);
    if (written != size)
        SR_DEBUG("uart: rx queue overflow detected");
}

/**
 * @brief Common RX event callback for all UART objects
 *
 * @param uart UART object
 * @param size data length that DMA receives
 */
void uart_callback_rx_event_common(uart_t *uart, uint16_t size)
{
    if(uart->hdma_rx) __HAL_DMA_DISABLE(uart->hdma_rx);
    uart_rx_internal(uart, size);
    if(uart->hdma_rx) __HAL_DMA_ENABLE(uart->hdma_rx);

    uart_rx_continue(uart);
}

/**
 * @brief Common RX completion callback for all UART objects
 *
 * @param uart UART object
 */
void uart_callback_rxcplt_common(uart_t *uart)
{
    uart_rx_internal(uart, uart->data_buffer_rx_size);

    uart_rx_continue(uart);
}

/**
 * @brief Common TX completion callback for all UART objects
 *
 * @param uart UART object
 */
void uart_callback_txcplt_common(uart_t *uart)
{
    if(uart->cb_txcomplete) uart->cb_txcomplete();
}

/**
 * @brief Log uart error
 * @param ec Errorcode to log
 */
void uart_log_error(uint32_t ec)
{
    SR_DEBUG("uart: error");
}

/**
 * @brief common error callback for all UART objects
 *
 * @param uart UART object
 */
void uart_callback_error_common(uart_t *uart)
{
    if(uart->cb_txcomplete) uart->cb_txcomplete();

#ifdef UART_LOG_ERROR_AT_INTERRUPT
    uart_log_error(uart->huart->ErrorCode);
#endif
    uart->last_error |= uart->huart->ErrorCode;

    uart_rx_continue(uart);
}

/**
 * @brief Get accumulated last errorcode (can be passed to uart_log_error)
 * @param uart UART object
 * @param clear true if last error should be cleared, false if otherwise
 * @return Accumulated last errorcode (truthy, regardless of underlying implementation ie. return 0 = no error)
 */
uint32_t uart_get_lasterror(uart_t *uart, bool clear)
{
    uint32_t errorcode = uart->last_error;
    if(clear) uart->last_error = HAL_UART_ERROR_NONE;
    return errorcode;
}

/**
 * @brief Check if RX data is available
 *
 * @param uart UART object
 * @return true when RX data is available. Otherwise, false
 */
bool uart_is_rx_available(uart_t *uart)
{
    return !queue_is_empty(&uart->queue_rx);
}

/**
 * @brief Get data from RX queue
 *
 * @param uart UART object
 * @return RX data
 */
uint8_t uart_getc(uart_t *uart)
{
    uint8_t c;

    queue_pop(&uart->queue_rx, &c, sizeof(c));

    return c;
}

/**
 * @brief Get uint16_t from buffer
 * @param uart UART object
 * @return RX data (halfword)
 */
uint16_t uart_get_u16(uart_t *uart)
{
    uint16_t c;

    queue_pop(&uart->queue_rx, &c, sizeof(c));

    return c;
}