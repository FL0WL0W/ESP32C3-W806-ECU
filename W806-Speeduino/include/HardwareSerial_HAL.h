/**
 * @file HardwareSerial.h
 *
 * @brief   HardwareSerial Module
 *
 * @author Huang Leilei
 *
 * Copyright (c) 2019 Winner Microelectronics Co., Ltd.
 */
#ifndef _HARDWARESERIAL_HAL_H_
#define _HARDWARESERIAL_HAL_H_

#include "HardwareSerial.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include "driver/wm_uart.h"

    void HAL_UART_MspInit(UART_HandleTypeDef *huart);
    // void UART1_Init(int baud);
    void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
    void uart_pins_init(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
extern UART_HandleTypeDef huart0;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;

extern UART_HandleTypeDef *uart_devices[UART_COUNT];
#endif
#endif
