#include "stm32h7xx_hal.h"
#include "stm32_def.h"

#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define RMII_MDC_Pin GPIO_PIN_1
#define RMII_MDC_GPIO_Port GPIOC
#define RMII_REF_CLK_Pin GPIO_PIN_1
#define RMII_REF_CLK_GPIO_Port GPIOA
#define RMII_MDIO_Pin GPIO_PIN_2
#define RMII_MDIO_GPIO_Port GPIOA
#define RMII_CRS_DV_Pin GPIO_PIN_7
#define RMII_CRS_DV_GPIO_Port GPIOA
#define RMII_RXD0_Pin GPIO_PIN_4
#define RMII_RXD0_GPIO_Port GPIOC
#define RMII_RXD1_Pin GPIO_PIN_5
#define RMII_RXD1_GPIO_Port GPIOC
#define LED_GREEN_Pin GPIO_PIN_0
#define LED_GREEN_GPIO_Port GPIOB
#define RMII_TXD1_Pin GPIO_PIN_13
#define RMII_TXD1_GPIO_Port GPIOB
#define LED_RED_Pin GPIO_PIN_14
#define LED_RED_GPIO_Port GPIOB
#define STLK_VCP_RX_Pin GPIO_PIN_8
#define STLK_VCP_RX_GPIO_Port GPIOD
#define STLK_VCP_TX_Pin GPIO_PIN_9
#define STLK_VCP_TX_GPIO_Port GPIOD
#define USB_FS_PWR_EN_Pin GPIO_PIN_10
#define USB_FS_PWR_EN_GPIO_Port GPIOD
#define USB_FS_OVCR_Pin GPIO_PIN_7
#define USB_FS_OVCR_GPIO_Port GPIOG
#define USB_FS_VBUS_Pin GPIO_PIN_9
#define USB_FS_VBUS_GPIO_Port GPIOA
#define USB_FS_ID_Pin GPIO_PIN_10
#define USB_FS_ID_GPIO_Port GPIOA
#define USB_FS_DM_Pin GPIO_PIN_11
#define USB_FS_DM_GPIO_Port GPIOA
#define USB_FS_DP_Pin GPIO_PIN_12
#define USB_FS_DP_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define RMII_TX_EN_Pin GPIO_PIN_11
#define RMII_TX_EN_GPIO_Port GPIOG
#define RMII_TXD0_Pin GPIO_PIN_13
#define RMII_TXD0_GPIO_Port GPIOG
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define LED_YELLOW_Pin GPIO_PIN_1
#define LED_YELLOW_GPIO_Port GPIOE

DMA_HandleTypeDef hdma_uart5_rx;

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    if (huart->Instance == UART5) {

        /** Initializes the peripherals clock
        */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART5;
        PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
            Error_Handler();
        }

        /* Peripheral clock enable */
        __HAL_RCC_UART5_CLK_ENABLE();

        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**UART5 GPIO Configuration
        PC12     ------> UART5_TX
        PB5     ------> UART5_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF14_UART5;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* UART5 DMA Init */
        /* UART5_RX Init */
        hdma_uart5_rx.Instance = DMA1_Stream0;
        hdma_uart5_rx.Init.Request = DMA_REQUEST_UART5_RX;
        hdma_uart5_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_uart5_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_uart5_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_uart5_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_uart5_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_uart5_rx.Init.Mode = DMA_NORMAL;
        hdma_uart5_rx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_uart5_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_uart5_rx) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(huart, hdmarx, hdma_uart5_rx);

//        /* UART5 interrupt Init */
//        HAL_NVIC_SetPriority(UART5_IRQn, 0, 0);
//        HAL_NVIC_EnableIRQ(UART5_IRQn);
    } else if (huart->Instance == USART3) {
        /** Initializes the peripherals clock
        */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
        PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
            Error_Handler();
        }

        /* Peripheral clock enable */
        __HAL_RCC_USART3_CLK_ENABLE();

        __HAL_RCC_GPIOD_CLK_ENABLE();
        /**USART3 GPIO Configuration
        PD8     ------> USART3_TX
        PD9     ------> USART3_RX
        */
        GPIO_InitStruct.Pin = STLK_VCP_RX_Pin | STLK_VCP_TX_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    }

}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart) {
    if (huart->Instance == UART5) {
        /* Peripheral clock disable */
        __HAL_RCC_UART5_CLK_DISABLE();

        /**UART5 GPIO Configuration
        PC12     ------> UART5_TX
        PB5     ------> UART5_RX
        */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_12);

        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5);

        /* UART5 DMA DeInit */
        HAL_DMA_DeInit(huart->hdmarx);
    } else if (huart->Instance == USART3) {
        /* Peripheral clock disable */
        __HAL_RCC_USART3_CLK_DISABLE();

        /**USART3 GPIO Configuration
        PD8     ------> USART3_TX
        PD9     ------> USART3_RX
        */
        HAL_GPIO_DeInit(GPIOD, STLK_VCP_RX_Pin | STLK_VCP_TX_Pin);
    }

}

/**
  * @brief This function handles DMA1 stream0 global interrupt.
  */
void DMA1_Stream0_IRQHandler(void) {
    HAL_DMA_IRQHandler(&hdma_uart5_rx);
}

/**
  * @brief This function handles DMAMUX1 overrun interrupt.
  */
void DMAMUX1_OVR_IRQHandler(void) {
    __NOP();
}


///**
//  * @brief This function handles UART5 global interrupt.
//  */
//void UART5_IRQHandler(void) {
//    HAL_UART_IRQHandler(&huart5);
//}

/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void) {
    /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

    /* USER CODE END NonMaskableInt_IRQn 0 */
    /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
    while (1) {
    }
    /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void) {
    /* USER CODE BEGIN HardFault_IRQn 0 */

    /* USER CODE END HardFault_IRQn 0 */
    while (1) {
        /* USER CODE BEGIN W1_HardFault_IRQn 0 */
        /* USER CODE END W1_HardFault_IRQn 0 */
    }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void) {
    /* USER CODE BEGIN MemoryManagement_IRQn 0 */

    /* USER CODE END MemoryManagement_IRQn 0 */
    while (1) {
        /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
        /* USER CODE END W1_MemoryManagement_IRQn 0 */
    }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void) {
    /* USER CODE BEGIN BusFault_IRQn 0 */

    /* USER CODE END BusFault_IRQn 0 */
    while (1) {
        /* USER CODE BEGIN W1_BusFault_IRQn 0 */
        /* USER CODE END W1_BusFault_IRQn 0 */
    }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void) {
    /* USER CODE BEGIN UsageFault_IRQn 0 */

    /* USER CODE END UsageFault_IRQn 0 */
    while (1) {
        /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
        /* USER CODE END W1_UsageFault_IRQn 0 */
    }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void) {
    /* USER CODE BEGIN SVCall_IRQn 0 */

    /* USER CODE END SVCall_IRQn 0 */
    /* USER CODE BEGIN SVCall_IRQn 1 */

    /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void) {
    /* USER CODE BEGIN DebugMonitor_IRQn 0 */

    /* USER CODE END DebugMonitor_IRQn 0 */
    /* USER CODE BEGIN DebugMonitor_IRQn 1 */

    /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void) {
    /* USER CODE BEGIN PendSV_IRQn 0 */

    /* USER CODE END PendSV_IRQn 0 */
    /* USER CODE BEGIN PendSV_IRQn 1 */

    /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void) {
    /* USER CODE BEGIN SysTick_IRQn 0 */

    /* USER CODE END SysTick_IRQn 0 */
    HAL_IncTick();
    /* USER CODE BEGIN SysTick_IRQn 1 */

    /* USER CODE END SysTick_IRQn 1 */
}
