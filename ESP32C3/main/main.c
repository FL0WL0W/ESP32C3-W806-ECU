#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "driver/usb_serial_jtag.h"
#include "hal/gpio_hal.h"
#include "UPDI.h"
#include "uart_listen.h"
#include "can_listen.h"
#include "sock_uart.h"
#include "sock_can.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#define W806_UART_RX_PIN 20
#define W806_UART_TX_PIN 21
#define W806_RESET_PIN 10
#define W806_BOOT_PIN 7

#define UPDI_KEYNVMPROG 0x4E564D50726F6720
#define UPDI_KEYNVMERASE 0x4E564D4572617365

extern const uint8_t attinyload_start[] asm("_binary_expander_bin_start");
extern const uint8_t attinyload_end[] asm("_binary_expander_bin_end");
// const size_t attinyload_bytes = attinyload_end - attinyload_start;

void _UPDI_error_check_failed(const char *file, int line, const char *function, const char *expression)
{
    ESP_LOGE("UPDI", "UPDI_ERROR_CHECK failed at %p", __builtin_return_address(0));
    ESP_LOGE("UPDI", "file: \"%s\" line %d\nfunc: %s\nexpression: %s", file, line, function, expression);
}

#define UPDI_ERROR_CHECK(x) do {                            \
        if (!(x)) {                                         \
            _UPDI_error_check_failed(__FILE__, __LINE__,    \
                                    __ASSERT_FUNC, #x);     \
            return false;                                   \
        }                                                   \
    } while(0)

uart_port_t UPDI_uart_num;

//read UPDI byte
bool UPDI_Read(uint8_t *val)
{
    return uart_read_bytes(UPDI_uart_num, val, 1, pdMS_TO_TICKS(100)) > 0;
}

//write UPDI byte
bool UPDI_Write(uint8_t val)
{
    return uart_write_bytes(UPDI_uart_num, &val, 1) > 0;
}

//UPDI send Break
void UPDI_Break()
{
    uint32_t baudrate = 0;

    uart_wait_tx_done(UPDI_uart_num, pdMS_TO_TICKS(30)); //wait for all bytes to be flushed
    uart_get_baudrate(UPDI_uart_num, &baudrate);
    uart_set_baudrate(UPDI_uart_num, 10000);
    UPDI_Write(0);  // send a zero byte
    uart_wait_tx_done(UPDI_uart_num, pdMS_TO_TICKS(30)); // wait for 0 byte to finish before restore normal baudrate
    uart_flush_input(UPDI_uart_num);
    uart_set_baudrate(UPDI_uart_num, baudrate); // set baudrate back to normal after break is sent
}

void UPDI_Idle()
{
    uint32_t baudrate = 0;
    uart_get_baudrate(1, &baudrate);
    esp_rom_delay_us(15000000/baudrate);
}

bool ack_disabled = false;
bool UPDI_ReadAck()
{
    if(ack_disabled) return true;
    uint8_t ack;
    if(!UPDI_Read(&ack)) return false;
    return ack == 0x40;
}

bool UPDI_DisableAck()
{
    uint8_t CTRLA = 0x0C;
    UPDI_ERROR_CHECK(UPDI_LDCS(0x2, &CTRLA));
    CTRLA |= 0x08;
    UPDI_STCS(0x2, CTRLA);
    ack_disabled = true;
    uint8_t CTRLACheck = 0x0C;
    UPDI_ERROR_CHECK(UPDI_LDCS(0x2, &CTRLACheck));
    UPDI_ERROR_CHECK(CTRLACheck == CTRLA);
    return true;
}

bool UPDI_EnableAck()
{
    uint8_t CTRLA = 0x06;
    UPDI_ERROR_CHECK(UPDI_LDCS(0x2, &CTRLA));
    CTRLA &= !0x08;
    UPDI_STCS(0x2, CTRLA);
    ack_disabled = false;
    uint8_t CTRLACheck = 0x06;
    UPDI_ERROR_CHECK(UPDI_LDCS(0x2, &CTRLACheck));
    UPDI_ERROR_CHECK(CTRLACheck == CTRLA);
    return true;
}

bool UPDI_Enable(uart_port_t uart_num, gpio_num_t tx_pin, gpio_num_t rx_pin)
{
    //start UPDI
    gpio_set_level(tx_pin, 1);
    gpio_set_direction(tx_pin, GPIO_MODE_OUTPUT);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[tx_pin], PIN_FUNC_GPIO);
    gpio_set_direction(rx_pin, GPIO_MODE_INPUT);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[rx_pin], PIN_FUNC_GPIO);

    //send enable pulse
    gpio_set_level(tx_pin, 0);
    gpio_set_level(tx_pin, 1);

    // //wait for enable pulse end
    uint32_t i = 0;
    while(!gpio_get_level(rx_pin) && i++ < 15) esp_rom_delay_us(100);

    // Setup UPDI UART
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_EVEN,
        .stop_bits = UART_STOP_BITS_2, 
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(uart_num, 1024, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_set_rx_full_threshold(uart_num, 1));
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    gpio_set_pull_mode(rx_pin, GPIO_FLOATING);

    UPDI_uart_num = uart_num;

    //send NVMPROG Key to enable UPDI
    uint64_t key = UPDI_KEYNVMPROG;
    UPDI_KeySend64((uint8_t *)&key);
    UPDI_Break();
    UPDI_Break();
    UPDI_Idle();

    //for some reason the break setting the baud rate back doesn't work. so do it here.
    uart_set_baudrate(1, 115200);

    //remove guard time and verify
    ack_disabled = false;
    uint8_t CTRLA = 0x06;
    UPDI_STCS(0x2, CTRLA);
    UPDI_ERROR_CHECK(UPDI_LDCS(0x2, &CTRLA));
    UPDI_ERROR_CHECK(CTRLA == 0x06);

    //disable collision detection and verify
    uint8_t CTRLB = 0x08;
    UPDI_STCS(0x3, CTRLB);
    UPDI_ERROR_CHECK(UPDI_LDCS(0x3, &CTRLB));
    UPDI_ERROR_CHECK(CTRLB == 0x08);

    return true;
}

bool UPDI_Reset() 
{
    UPDI_STCS(0x8, 0x59);
    UPDI_STCS(0x8, 0x0);
    uint8_t ASI_SYS_STATUS = 0;
    uint32_t i = 0;
    UPDI_ERROR_CHECK(UPDI_LDCS(0xB, &ASI_SYS_STATUS));
    while((ASI_SYS_STATUS & 0xE) == 0 && i++<configTICK_RATE_HZ) 
    {
        vTaskDelay(1);
        UPDI_ERROR_CHECK(UPDI_LDCS(0xB, &ASI_SYS_STATUS));
    }
    return (ASI_SYS_STATUS & 0xE) != 0;
}

bool UPDI_EraseChip() 
{
    uint64_t key = UPDI_KEYNVMERASE;
    UPDI_KeySend64((uint8_t *)&key);
    esp_rom_delay_us(100);
    return UPDI_Reset();
}

bool UPDI_NVMProg() 
{
    uint64_t key = UPDI_KEYNVMPROG;
    UPDI_KeySend64((uint8_t *)&key);
    esp_rom_delay_us(100);
    UPDI_ERROR_CHECK(UPDI_Reset());

    uint8_t ASI_SYS_STATUS = 0;
    uint32_t i = 0;
    UPDI_ERROR_CHECK(UPDI_LDCS(0xB, &ASI_SYS_STATUS));
    while(!(ASI_SYS_STATUS & 0x8) && i++<configTICK_RATE_HZ/50) 
    {
        vTaskDelay(1);
        UPDI_ERROR_CHECK(UPDI_LDCS(0xB, &ASI_SYS_STATUS));
    }
    return ASI_SYS_STATUS & 0x8;
}

bool UPDI_WriteFlashOrEEPROM(uint32_t address, uint8_t *data, uint32_t length) 
{
    UPDI_ERROR_CHECK(UPDI_STSB(0x1000, 0x4));

    //program
    UPDI_DisableAck();
    uint32_t addressI = address;
    uint8_t *dataI = data;
    uint32_t lengthI = length;
    while(lengthI > 0) 
    {
        //wait for ready
        uint8_t NVMCTRL_STATUS = 0;
        uint32_t i = 0;
        UPDI_ERROR_CHECK(UPDI_LDSB(0x1002, &NVMCTRL_STATUS));
        while((NVMCTRL_STATUS & 0x3) != 0 && i++<1000) 
        {
            esp_rom_delay_us(100);
            UPDI_ERROR_CHECK(UPDI_LDSB(0x1002, &NVMCTRL_STATUS));
        }
        UPDI_ERROR_CHECK((NVMCTRL_STATUS & 0x3) == 0);

        uint32_t pagelength = 64 - (addressI % 64);
        if(lengthI < pagelength)
            pagelength = lengthI;
        
        UPDI_ERROR_CHECK(UPDI_ST(addressI, dataI, pagelength));

        UPDI_ERROR_CHECK(UPDI_STSB(0x1000, 0x3));

        addressI += pagelength;
        dataI += pagelength;
        lengthI -= pagelength;
    }
    UPDI_EnableAck();

    //verify
    uint8_t verifybuf[length];
    UPDI_ERROR_CHECK(UPDI_LD(address, verifybuf, length));
    addressI = address;
    if(memcmp(verifybuf, data, length) != 0)
    {
        ESP_LOGE("UPDI", "UPDI verification failed");
        for(int a = 0; a < length; a+=16) 
        {
            ESP_LOGE("UPDI", "%04X\t%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X| %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
                (unsigned int)addressI,
                verifybuf[a],   verifybuf[a+1], verifybuf[a+2],  verifybuf[a+3],  verifybuf[a+4],  verifybuf[a+5],  verifybuf[a+6],  verifybuf[a+7],
                verifybuf[a+8], verifybuf[a+9], verifybuf[a+10], verifybuf[a+11], verifybuf[a+12], verifybuf[a+13], verifybuf[a+14], verifybuf[a+15],
                data[a],   data[a+1], data[a+2],  data[a+3],  data[a+4],  data[a+5],  data[a+6],  data[a+7],
                data[a+8], data[a+9], data[a+10], data[a+11], data[a+12], data[a+13], data[a+14], data[a+15]);
            addressI += 16;
        }
        return false;
    }

    // uint8_t verifybuf[512];
    // addressI = address;
    // dataI = data;
    // lengthI = length;
    // while(lengthI > 0) 
    // {
    //     uint32_t pagelength = 512 - (addressI % 512);
    //     if(lengthI < pagelength)
    //         pagelength = lengthI;

    //     UPDI_ERROR_CHECK(UPDI_LD(addressI, verifybuf, pagelength));

    //     if(memcmp(verifybuf, dataI, pagelength) != 0)
    //     {
    //         ESP_LOGE("UPDI", "UPDI verification failed @ %04X", (unsigned int)addressI);
    //         for(int a = 0; a < pagelength; a+=16) 
    //         {
    //             ESP_LOGE("UPDI", "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X| %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
    //                 verifybuf[a],   verifybuf[a+1], verifybuf[a+2],  verifybuf[a+3],  verifybuf[a+4],  verifybuf[a+5],  verifybuf[a+6],  verifybuf[a+7],
    //                 verifybuf[a+8], verifybuf[a+9], verifybuf[a+10], verifybuf[a+11], verifybuf[a+12], verifybuf[a+13], verifybuf[a+14], verifybuf[a+15],
    //                 dataI[a],   dataI[a+1], dataI[a+2],  dataI[a+3],  dataI[a+4],  dataI[a+5],  dataI[a+6],  dataI[a+7],
    //                 dataI[a+8], dataI[a+9], dataI[a+10], dataI[a+11], dataI[a+12], dataI[a+13], dataI[a+14], dataI[a+15]);
    //         }
    //         return false;
    //     }

    //     addressI += pagelength;
    //     dataI += pagelength;
    //     lengthI -= pagelength;
    // }

    return true;
}

bool UPDI_Program(uart_port_t uart_num, gpio_num_t tx_pin, gpio_num_t rx_pin, uint8_t *data, uint32_t length)
{
    ESP_LOGI("UPDI", "programming attiny");
    UPDI_ERROR_CHECK(UPDI_Enable(uart_num, tx_pin, rx_pin));
    if(!UPDI_EraseChip()) UPDI_ERROR_CHECK(UPDI_EraseChip());
    if(!UPDI_NVMProg()) UPDI_ERROR_CHECK(UPDI_NVMProg());
    UPDI_ERROR_CHECK(UPDI_WriteFlashOrEEPROM(0x8000, data, length));
    UPDI_ERROR_CHECK(UPDI_Reset());
    ESP_LOGI("UPDI", "success!");
    return true;
}

void wifi_init_softap()
{
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "EFIGenie",
            .ssid_len = strlen("EFIGenie"),
            // .password = EXAMPLE_ESP_WIFI_PASS,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = { .required = true },
            .max_connection = 5
        },
    };
    if (strlen((char *)wifi_config.ap.password) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void app_main()
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    //initialize net
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    //initialize wifi
    wifi_init_softap();

    //reset w806
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL<<W806_RESET_PIN) | (1ULL<<W806_BOOT_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_ERROR_CHECK(gpio_set_level(W806_RESET_PIN, 0));
    ESP_ERROR_CHECK(gpio_set_level(W806_BOOT_PIN, 1));
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_ERROR_CHECK(gpio_set_level(W806_RESET_PIN, 1));

    //install uart listen services
    uart_listen_config_t uart_listen_config[UART_NUM_MAX];
    char uart_listen_name[UART_NUM_MAX][16];
    for(uint8_t i = UART_NUM_0; i < UART_NUM_MAX; i++)
    {
        uart_listen_config[i].uart_num = i;
        uart_listen_config[i].rx_buffer_size = 2048;
        uart_listen_config[i].tx_buffer_size = 0;
        sprintf(uart_listen_name[i], "uart_listen_%d", i);
        xTaskCreate(uart_listen, uart_listen_name[i], 4096, &uart_listen_config[i], 10, NULL);
    }

    uart_config_t W806_uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    sock_uart_config_t W806_sock_uart_config = 
    {
        .port = 8000,
        .sock_rx_buffer_size = 1440,
        .uart_num = 0,
        .uart_config = &W806_uart_config,
        .rx_pin = W806_UART_RX_PIN,
        .tx_pin = W806_UART_TX_PIN
    };

    xTaskCreate(sock_uart, "w806_sock_uart", 4096, &W806_sock_uart_config, 5, NULL);

    // //install can listen service
    // twai_general_config_t twai_general_config = {
    //     .controller_id = 0,
    //     .mode = TWAI_MODE_LISTEN_ONLY,
    //     .tx_io = 9,
    //     .rx_io = 8,
    //     .clkout_io = TWAI_IO_UNUSED,
    //     .bus_off_io = TWAI_IO_UNUSED,
    //     .tx_queue_len = 1000,
    //     .rx_queue_len = 1000,
    //     .alerts_enabled = TWAI_ALERT_NONE,
    //     .clkout_divider = 0,
    //     .intr_flags = ESP_INTR_FLAG_LEVEL1
    // };
    // twai_timing_config_t twai_timing_config = TWAI_TIMING_CONFIG_500KBITS();
    // twai_filter_config_t twai_filter_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    // twai_handle_t twai_handle;
    // twai_driver_install_v2(&twai_general_config, &twai_timing_config, &twai_filter_config, &twai_handle);
    // can_listen_config_t can_listen_config = {
    //     .can_num = 0,
    //     .can_handle = twai_handle
    // };

    // xTaskCreate(can_listen, "can_listen", 4096, &can_listen_config, 9, NULL);

    // sock_can_config_t sock_can_config = {
    //     .port = 7000,
    //     .can_num = 0,
    //     .can_handle = twai_handle,
    //     .can_general_config = &twai_general_config,
    //     .can_timing_config = &twai_timing_config,
    //     .can_filter_config = &twai_filter_config
    // };
    // xTaskCreate(can_listen, "sock_can", 4096, &sock_can_config, 4, NULL);

    // vTaskDelay(pdMS_TO_TICKS(1000));
    // size_t attinyload_bytes = attinyload_end - attinyload_start;
    // uint8_t *attinyload = (uint8_t *)calloc(attinyload_bytes, sizeof(uint8_t));
    // memcpy(attinyload, attinyload_start, attinyload_bytes);
    // UPDI_Program(4, attinyload, attinyload_bytes);
    
    // xTaskCreate(echo_task, "echo_task", 2048, NULL, 10, NULL);

    while(1) {
        vTaskDelay(1);
    }
}
