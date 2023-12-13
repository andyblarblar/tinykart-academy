#pragma once

#include "stm32h7xx_hal.h"
#include "functional"
#include "uart.hpp"

/// Wraps a uart port providing accelerated reads.
/// This should only be used through the predefined globals.
class DMASerialRx {
    /// Handle to HAL UART perf
    UART_HandleTypeDef *raw_serial;

    static void MX_DMA_Init();

    void MX_UART_Init(uint32_t baud);

public:
    /// User supplied callback called on each serial transfer completion.
    std::function<void(volatile uint8_t *)> cb;

    void init(uint32_t baud, std::function<void(volatile uint8_t *)> rx_callback);

    explicit DMASerialRx(UART_HandleTypeDef *huart, USART_TypeDef *uart);
};

// Initialize singletons for each peripheral (like normal Arduino)
// Note that when these are created they will take ownership of that uart.

/// UART DMA receiver for UART5.
inline DMASerialRx dmaSerialRx5{&huart5, UART5};