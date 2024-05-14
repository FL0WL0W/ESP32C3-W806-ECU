/******************************************************************************
** 
 * \file        main.c
 * \author      IOsetting | iosetting@outlook.com
 * \date        
 * \brief       Demo code of PWM in independent mode
 * \note        This will drive 3 on-board LEDs to show fade effect
 * \version     v0.1
 * \ingroup     demo
 * \remarks     test-board: HLK-W806-KIT-V1.0
 *              PWM Frequency = 40MHz / Prescaler / (Period + 1)；
                Duty Cycle(Edge Aligned)   = (Pulse + 1) / (Period + 1)
                Duty Cycle(Center Aligned) = (2 * Pulse + 1) / (2 * (Period + 1))
 *
******************************************************************************/

#include <stdio.h>
#include "wm_hal.h"

void Error_Handler(void);

SPI_HandleTypeDef hspi;
DMA_HandleTypeDef hdma_spi_tx;
DMA_HandleTypeDef hdma_spi_rx;

static void SPI_Init(void);
static void DMA_Init(void);
#define data_len (10000)
uint8_t tx_data[data_len] = {0};
uint8_t rx_data[data_len] = {0};

int main(void)
{
    SystemClock_Config(CPU_CLK_240M);
    printf("enter main\r\n");
    DMA_Init();
    SPI_Init();
    
    __HAL_RCC_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);

    //adc channels enabled
    tx_data[0] = 0xE4;
    tx_data[1] = 0x1C;
    tx_data[2] = 0xE0;//analog enable pins 5-7
    tx_data[3] = 0x01;//analog enable pins 8, 10-15
    tx_data[4] = 0x0F;// analog accumulate to 16 bits
    tx_data[5] = 0x00;//

    //setup ADC
    tx_data[6] = 0x85; //write 5 bytes to 16 bit address
    tx_data[7] = 0x06; //address high
    tx_data[8] = 0x00; //address low
    tx_data[9] = 0b00100001; // enable with low latency
    tx_data[10] = 0x01; // prescaler DIV4 to get ADCCLK 5MHZ
    tx_data[11] = 0xA0; // set timebase and VDD as reference
    tx_data[12] = 0x00; // no window source mode
    tx_data[13] = 0x01; //enable RESRDY intterupt

    //start ADC
    tx_data[14] = 0xC3; //write 3 bytes to 8 bit address using existing high byte
    tx_data[15] = 0x08; //address low
    tx_data[16] = 0x05; //SAMPDUR = 5. this give a sample duration of 4us
    tx_data[17] = 0x00; //no accumulation, accumulation done in software so the readings are evenly spaced
    tx_data[18] = 0x11; //single 12 bit mode and start

    //dummy
    tx_data[19] = 0x00;
    tx_data[20] = 0x00;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi, (uint8_t *)tx_data, (uint8_t *)rx_data, 21, 1000);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
    printf(
//"tx: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n
"rx: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n", 
        // tx_data[0], tx_data[1], tx_data[2], tx_data[3], tx_data[4], tx_data[5], tx_data[6], tx_data[7], tx_data[8], tx_data[9], tx_data[10], tx_data[11],
        // tx_data[12], tx_data[13], tx_data[14], tx_data[15], tx_data[16], tx_data[17], tx_data[18], tx_data[19],
        rx_data[0], rx_data[1], rx_data[2], rx_data[3], rx_data[4], rx_data[5], rx_data[6], rx_data[7], rx_data[8], rx_data[9], rx_data[10], rx_data[11],
        rx_data[12], rx_data[13], rx_data[14], rx_data[15], rx_data[16], rx_data[17], rx_data[18], rx_data[19]);

    while (1)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
        tx_data[0] = 0x64;
        tx_data[1] = 0x1C;
        tx_data[2] = 0;
        tx_data[3] = 0;
        tx_data[4] = 0;
        tx_data[5] = 0;
        tx_data[6] = 0x15;
        tx_data[7] = 0x34;
        tx_data[8] = 0x0b;
        tx_data[9] = 0;
        tx_data[10] = 0;
        tx_data[11] = 0;
        tx_data[12] = 0;
        tx_data[13] = 0;
        tx_data[14] = 0;
        tx_data[15] = 0;
        tx_data[16] = 0;
        tx_data[17] = 0;
        tx_data[18] = 0;
        tx_data[19] = 0;
        tx_data[20] = 0;
        tx_data[21] = 0;
        tx_data[22] = 0;
        tx_data[23] = 0;
        tx_data[24] = 0;
        tx_data[25] = 0;
        tx_data[26] = 0;
        tx_data[27] = 0;
        tx_data[28] = 0;
        tx_data[29] = 0;
        tx_data[30] = 0;
        tx_data[31] = 0;

        HAL_SPI_TransmitReceive(&hspi, (uint8_t *)tx_data, (uint8_t *)rx_data, 32, 1000);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
        printf("\r\nrx: ");
        printHexLine(rx_data);
        printHexLine(&rx_data[16]);
    }
}

printHexLine(uint8_t *buf) {
    printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ",
            buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8],buf[9],buf[10],buf[11],buf[12],buf[13],buf[14],buf[15]);
}

static void SPI_Init(void)
{
    hspi.Instance = SPI;
    hspi.Init.Mode = SPI_MODE_MASTER;
    hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi.Init.NSS = SPI_NSS_SOFT;
    hspi.Init.BaudRatePrescaler = 7;
    hspi.Init.FirstByte = SPI_LITTLEENDIAN;
    
    if (HAL_SPI_Init(&hspi) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    // __HAL_SPI_SET_CS_HIGH(hspi);
    // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
    // printf("rx: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n", 
    //     rx_data[0], rx_data[1], rx_data[2], rx_data[3], rx_data[4], rx_data[5], rx_data[6], rx_data[7], rx_data[8], rx_data[9]);
}

static void DMA_Init(void)
{
    __HAL_RCC_DMA_CLK_ENABLE();
    
    HAL_NVIC_SetPriority(DMA_Channel0_IRQn, 0);
    HAL_NVIC_EnableIRQ(DMA_Channel0_IRQn);
    
    HAL_NVIC_SetPriority(DMA_Channel1_IRQn, 0);
    HAL_NVIC_EnableIRQ(DMA_Channel1_IRQn);
}

void Error_Handler(void)
{
    while (1)
    {
    }
}

void assert_failed(uint8_t *file, uint32_t line)
{
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}