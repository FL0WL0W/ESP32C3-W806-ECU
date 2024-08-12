
#include "wm_hal.h"

#define readl(addr) ({unsigned int __v = (*(volatile unsigned int *) (addr)); __v;})
__attribute__((isr)) void CORET_IRQHandler(void)
{
    readl(0xE000E010);
    HAL_IncTick();
}

extern UART_HandleTypeDef huart0;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;

__attribute__((isr)) void UART0_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart0);
}

__attribute__((isr)) void UART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}

__attribute__((isr)) void UART2_5_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);
    HAL_UART_IRQHandler(&huart3);
    HAL_UART_IRQHandler(&huart4);
    HAL_UART_IRQHandler(&huart5);
}