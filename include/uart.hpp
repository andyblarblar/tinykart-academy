#pragma once

#include "stm32h7xx_hal.h"

// Extern to peripherals in the msp init, which must be there for access to IRQs. They are not initialized yet.

extern UART_HandleTypeDef huart5;
extern DMA_HandleTypeDef hdma_uart5_rx;

extern UART_HandleTypeDef huart3;