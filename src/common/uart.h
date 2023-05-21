#ifndef __UART_H__
#define __UART_H__

#include "hal_stm32.h"

#include "queue.h"

typedef void (*uart_txcompletion_func_t)(void);
typedef  void (*uart_internal_init)(void);

#define UART_RX_QUEUE_SIZE      1024

typedef struct {
    UART_HandleTypeDef *huart;
    DMA_HandleTypeDef *hdma_rx;             // NULL is use Interrupt Only

    uart_txcompletion_func_t cb_txcomplete;
    uart_internal_init internal_init;

        // Internal (thunk) callbacks
        // - These are used to transition HAL's handle to uart_t*, and are in-lieu of a
        // -  better methodology (for example a opaque 'user' pointer in the uart handle structure)
    pUART_CallbackTypeDef callback_error;
    pUART_CallbackTypeDef callback_txcplt;
    pUART_CallbackTypeDef callback_rxcplt;
    pUART_RxEventCallbackTypeDef callback_rx_event;

    char *data_buffer_rx;
    int data_buffer_rx_size;

    queue_t queue_rx;
    char *queue_buffer_rx;
    int queue_buffer_rx_size;

    uint32_t last_error;
} uart_t;

void uart_callback_error_common(uart_t *uart);
void uart_callback_rxcplt_common(uart_t *uart);
void uart_callback_txcplt_common(uart_t *uart);
void uart_callback_rx_event_common(uart_t *uart, uint16_t size);

#define uart_def(name, _huart, _hdma_rx, _data_buffer_rx_size, _queue_rx_size) \
\
    static void __##name##_uart_internal_init(); \
    static void __##name##_uart_callback_error(UART_HandleTypeDef *huart); \
    static void __##name##_uart_callback_txcplt(UART_HandleTypeDef *huart); \
    static void __##name##_uart_callback_rxcplt(UART_HandleTypeDef *huart); \
    static void __##name##_uart_callback_rx_event(UART_HandleTypeDef *huart, uint16_t size); \
\
    static char __##name##_data_buffer_rx[_data_buffer_rx_size]; \
    static char __##name##_queue_buffer_rx[_queue_rx_size]; \
\
    static uart_t __##name##_obj = { \
        .internal_init = __##name##_uart_internal_init, \
        .data_buffer_rx = __##name##_data_buffer_rx, \
        .data_buffer_rx_size = sizeof(__##name##_data_buffer_rx), \
        .queue_buffer_rx = __##name##_queue_buffer_rx, \
        .queue_buffer_rx_size = sizeof(__##name##_queue_buffer_rx), \
        .callback_error = __##name##_uart_callback_error, \
        .callback_txcplt = __##name##_uart_callback_txcplt,                    \
        .callback_rxcplt = __##name##_uart_callback_rxcplt, \
        .callback_rx_event = __##name##_uart_callback_rx_event, \
    }; \
\
    static uart_t *const name = &__##name##_obj; \
\
    static void __##name##_uart_callback_error(UART_HandleTypeDef *huart) \
    { \
        uart_callback_error_common(name); \
    } \
\
    static void __##name##_uart_callback_rxcplt(UART_HandleTypeDef *huart) \
    { \
        uart_callback_rxcplt_common(name); \
    } \
\
    static void __##name##_uart_callback_txcplt(UART_HandleTypeDef *huart) \
    { \
        uart_callback_txcplt_common(name); \
    } \
\
    static void __##name##_uart_callback_rx_event(UART_HandleTypeDef *huart, uint16_t size) \
    { \
        uart_callback_rx_event_common(name, size); \
    } \
\
    static void __##name##_uart_internal_init() \
    { \
        __##name##_obj.huart = _huart; \
        __##name##_obj.hdma_rx = _hdma_rx; \
    } \

void uart_init(uart_t *uart, bool autostart);
void uart_register_txcompletion(uart_t *uart, uart_txcompletion_func_t txcompletion_cb);

bool uart_is_rx_available(uart_t *uart);
uint8_t uart_getc(uart_t *uart);
uint16_t uart_get_u16(uart_t *uart);

    // Implementation Detail
void uart_register_internal_callbacks(uart_t *uart);
void uart_rx_continue(uart_t *uart);
void uart_rx_abort(uart_t *uart);
void uart_abort(uart_t *uart);

void uart_log_error(uint32_t ec);
uint32_t uart_get_lasterror(uart_t *uart, bool clear);

#endif /* __UART_H__ */
