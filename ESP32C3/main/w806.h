#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "uart_listen.h"

#ifndef W806_H
#define W806_H

#define W806_UART_NUM (uart_port_t)0
#define W806_UART_RX_PIN (gpio_num_t)17
#define W806_UART_TX_PIN (gpio_num_t)16
#define W806_RESET_PIN (gpio_num_t)23
#define W806_BOOT_PIN (gpio_num_t)2
#define W806_XMODEM_DATA_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

const static unsigned char wm_tool_chip_cmd_b115200[]  = {0x21, 0x0a, 0x00, 0x97, 0x4b, 0x31, 0x00, 0x00, 0x00, 0x00, 0xc2, 0x01, 0x00};
const static unsigned char wm_tool_chip_cmd_b460800[]  = {0x21, 0x0a, 0x00, 0x07, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x08, 0x07, 0x00};
const static unsigned char wm_tool_chip_cmd_b921600[]  = {0x21, 0x0a, 0x00, 0x5d, 0x50, 0x31, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0e, 0x00};
const static unsigned char wm_tool_chip_cmd_b1000000[] = {0x21, 0x0a, 0x00, 0x5e, 0x3d, 0x31, 0x00, 0x00, 0x00, 0x40, 0x42, 0x0f, 0x00};
const static unsigned char wm_tool_chip_cmd_b2000000[] = {0x21, 0x0a, 0x00, 0xef, 0x2a, 0x31, 0x00, 0x00, 0x00, 0x80, 0x84, 0x1e, 0x00};

const static unsigned char wm_tool_chip_cmd_get_mac[]  = {0x21, 0x06, 0x00, 0xea, 0x2d, 0x38, 0x00, 0x00, 0x00};
const static unsigned char wm_tool_chip_cmd_erase[]  = {0x21, 0x0a, 0x00, 0xc3, 0x35, 0x32, 0x00, 0x00, 0x00, 0x02, 0x00, 0xfe, 0x01};

#endif
static inline void reset_w806()
{
    ESP_LOGI("W806", "Resetting");
    ESP_ERROR_CHECK(gpio_set_level(W806_RESET_PIN, 0));
    ESP_ERROR_CHECK(gpio_set_direction(W806_RESET_PIN, GPIO_MODE_OUTPUT));
    vTaskDelay(pdMS_TO_TICKS(1));
    ESP_ERROR_CHECK(gpio_set_level(W806_RESET_PIN, 1));
}
static inline void bootset_w806()
{
    ESP_LOGI("W806", "Boot Set");
    ESP_ERROR_CHECK(gpio_set_level(W806_BOOT_PIN, 1));
    ESP_ERROR_CHECK(gpio_set_direction(W806_BOOT_PIN, GPIO_MODE_OUTPUT));
}
static inline void bootreset_w806()
{
    ESP_LOGI("W806", "Boot Reset");
    ESP_ERROR_CHECK(gpio_set_level(W806_BOOT_PIN, 0));
    ESP_ERROR_CHECK(gpio_set_direction(W806_BOOT_PIN, GPIO_MODE_OUTPUT));
}

static void reset_w806_task(void *arg)
{
    reset_w806();
    vTaskDelete(NULL);
}

static uint16_t w806_crc16(char *ptr, uint16_t count)
{
    uint16_t crc, i;

    crc = 0;

    while (count--)
    {
        crc = crc ^ ((int)*((unsigned char *)ptr++)) << 8;

        for (i = 0; i < 8; i++)
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        }
    }

    return (crc & 0xFFFF);
}

#ifdef __cplusplus
}
#endif