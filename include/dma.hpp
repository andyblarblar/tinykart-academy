#pragma once

#include "stm32h7xx_hal.h"
#include "functional"

class DMASerialRx {
    /// Handle to HAL UART perf
    UART_HandleTypeDef raw_serial;

    static void MX_DMA_Init();

    void MX_UART_Init(uint32_t baud);

public:
    /// User supplied callback called on each serial transfer completion. Note that this is not called in the IRQ, rather
    /// The hals request complete callback.
    std::function<void(uint8_t *)> cb;

    void init(uint32_t baud, std::function<void(uint8_t *)> rx_callback);

    // TODO once done fold this back into init
    void begin();

    explicit DMASerialRx(USART_TypeDef *uart);
};

// Initialize singletons for each peripheral (like normal Arduino)
inline DMASerialRx dmaSerialRx5{UART5};