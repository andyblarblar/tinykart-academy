#pragma once

#include "stm32h7xx_hal.h"
#include "functional"

// So close to not hardcoding :(
constexpr size_t DMA_LIDAR_BUFFER_SIZE = 47;
uint8_t dma_lidar_buffer[DMA_LIDAR_BUFFER_SIZE] = {0};

class DMASerialRx {
    /// Handle to HAL UART perf
    UART_HandleTypeDef raw_serial;

    static void MX_DMA_Init() {
        /* DMA controller clock enable */
        __HAL_RCC_DMA1_CLK_ENABLE();

        /* DMA interrupt init */
        /* DMA1_Stream0_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
    }

    void MX_UART_Init(uint32_t baud) {
        // TODO currently hardcoded to 5
        __HAL_RCC_UART5_FORCE_RESET();
        __HAL_RCC_UART5_RELEASE_RESET();
        __HAL_RCC_UART5_CLK_ENABLE();

        raw_serial.Init.BaudRate = baud;
        raw_serial.Init.WordLength = UART_WORDLENGTH_8B;
        raw_serial.Init.StopBits = UART_STOPBITS_1;
        raw_serial.Init.Parity = UART_PARITY_NONE;
        raw_serial.Init.Mode = UART_MODE_RX;
        raw_serial.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        raw_serial.Init.OverSampling = UART_OVERSAMPLING_16;
        raw_serial.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
        raw_serial.Init.ClockPrescaler = UART_PRESCALER_DIV1;
        raw_serial.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
        if (HAL_UART_Init(&raw_serial) != HAL_OK) {
            Error_Handler();
        }
        if (HAL_UARTEx_SetTxFifoThreshold(&raw_serial, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
            Error_Handler();
        }
        if (HAL_UARTEx_SetRxFifoThreshold(&raw_serial, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
            Error_Handler();
        }
        if (HAL_UARTEx_DisableFifoMode(&raw_serial) != HAL_OK) {
            Error_Handler();
        }
    }

public:
    /// User supplied callback called on each serial transfer completion.
    std::function<void(uint8_t *)> cb;

    void begin(uint32_t baud, std::function<void(uint8_t *)> rx_callback) {
        MX_DMA_Init();
        MX_UART_Init(baud);
        cb = std::move(rx_callback);

        // Call first DMA receive to start the train
        HAL_UART_Receive_DMA(&raw_serial, dma_lidar_buffer, DMA_LIDAR_BUFFER_SIZE);
    }

    explicit DMASerialRx(USART_TypeDef *uart) {
        raw_serial.Instance = uart;
    }
};

// Initialize singletons for each peripheral (like normal Arduino)
DMASerialRx dmaSerialRx5{UART5};

// Called on reach DMA finish
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    // Call user callback
    dmaSerialRx5.cb(dma_lidar_buffer);

    // Recurse
    HAL_UART_Receive_DMA(huart, dma_lidar_buffer, DMA_LIDAR_BUFFER_SIZE);
}
