#pragma once

#include <cstdarg>
#include "deque"
#include "stm32h7xx_hal.h"
#include "string_view"
#include "uart.hpp"

/// Buffered logging to USART3, which is connected to the debugger on nucleo boards.
/// This logger will asynchronously write messages over this uart, buffering them to avoid writing two messages at once.
/// Because of this, messages will have to wait in the buffer FIFO, adding a delay before transmission. If the buffer is
/// full, then messages are dropped.
class Logger {

    void MX_USART3_UART_Init() {
        huart3.Instance = USART3;
        huart3.Init.BaudRate = 115200;
        huart3.Init.WordLength = UART_WORDLENGTH_8B;
        huart3.Init.StopBits = UART_STOPBITS_1;
        huart3.Init.Parity = UART_PARITY_NONE;
        huart3.Init.Mode = UART_MODE_TX_RX;
        huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        huart3.Init.OverSampling = UART_OVERSAMPLING_16;
        huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
        huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
        huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
        if (HAL_UART_Init(&huart3) != HAL_OK) {
            Error_Handler();
        }
        if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
            Error_Handler();
        }
        if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
            Error_Handler();
        }
        if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK) {
            Error_Handler();
        }
    }

public:
    /// FIFO message queue
    std::deque<String> buff{};

    Logger() {
        MX_USART3_UART_Init();
    }

    bool printf(const char *format, ...) {
        bool erred = false;

        std::va_list argv;
        va_start(argv, format);

        // First format in local buffer
        char buffer[128];
        auto len = vsprintf(buffer, format, argv);

        // Add to queue, and start transmit train if nothing is being transmitted
        noInterrupts();
        // Arbitrarily bound queue
        if (buff.size() < 128) {
            buff.emplace_back(buffer);
            erred = true;
        }

        if (buff.size() == 1) {
            HAL_UART_Transmit_IT(&huart3, reinterpret_cast<const uint8_t *>(buff.front().c_str()),
                                 buff.front().length());
        }
        interrupts();

        va_end(argv);

        return !erred;
    }
};

/// Logs messages to USART3, connected to the usb port.
inline Logger logger{};

extern "C" {
// Called on each Tx finish
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART3) {
        logger.buff.pop_front();

        // Transmit next in buffer
        if (!logger.buff.empty()) {
            HAL_UART_Transmit_IT(&huart3, reinterpret_cast<const uint8_t *>(logger.buff.front().c_str()),
                                 logger.buff.front().length());
        }
    }
}
}