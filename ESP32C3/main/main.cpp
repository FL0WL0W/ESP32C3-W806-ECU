#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "hal/gpio_hal.h"
#include "uart_listen.h"
#include "sock_uart.h"
#include "mount.h"
#include "http_server.h"
#include <ATTiny_UPDI.h>

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "ExpanderMain.h"
#include "EmbeddedIOServiceCollection.h"
#include "Variable.h"
#include "CallBack.h"
#include "Config.h"
#include "CommunicationHandler_Prefix.h"
#include "CommunicationHandlers/CommunicationHandler_EFIGenie.h"
#include "Esp32IdfAnalogService.h"
#include "Esp32IdfDigitalService.h"
#include "Esp32IdfTimerService.h"
#include "Esp32IdfPwmService.h"
#include "Esp32IdfCANService.h"
#include "Esp32IdfCommunicationService_Socket.h"
#include "Esp32IdfCommunicationService_WebSocket.h"
#include "w806.h"

#define UPDI1_UART_RX_PIN (gpio_num_t)20
#define UPDI1_UART_TX_PIN (gpio_num_t)19
#define UPDI2_UART_RX_PIN (gpio_num_t)21
#define UPDI2_UART_TX_PIN (gpio_num_t)22

using namespace OperationArchitecture;
using namespace EmbeddedIOServices;
using namespace EmbeddedIOOperations;
using namespace Esp32;
using namespace EFIGenie;

//read UPDI byte
extern "C" bool UPDI_Read(uint8_t *val)
{
    uint8_t timeout = 0;
    while(UPDI_rx_buffer_length < 1 && timeout++ < 100)
        vTaskDelay(1);
    if(timeout > 99){
        return false;
    }
    // ESP_LOGE("UPDI", "read %x %d %d %d", UPDI_rx_buffer[UPDI_rx_buffer_index], UPDI_rx_buffer_index, UPDI_rx_buffer_length, timeout);
    *val = UPDI_rx_buffer[UPDI_rx_buffer_index++];
    UPDI_rx_buffer_length--;
    UPDI_rx_buffer_index %= UPDI_RX_BUFFER_LENGTH;
    return true;
}

//write UPDI byte
extern "C" bool UPDI_Write(uint8_t val)
{
    if(!(uart_write_bytes(UPDI_uart_num, &val, 1) > 0))
        return false;
    uint8_t verify;
    if(!UPDI_Read(&verify))
        return false;
    return verify == val;
}

//UPDI send Break
extern "C" void UPDI_Break()
{
    uint32_t baudrate = 100000;

    uart_wait_tx_done(UPDI_uart_num, pdMS_TO_TICKS(30)); //wait for all bytes to be flushed
    uart_get_baudrate(UPDI_uart_num, &baudrate);
    uart_set_baudrate(UPDI_uart_num, 300);
    UPDI_Write(0);  // send a zero byte
    uart_wait_tx_done(UPDI_uart_num, pdMS_TO_TICKS(30)); // wait for 0 byte to finish before restore normal baudrate
    uart_set_baudrate(UPDI_uart_num, baudrate); // set baudrate back to normal after break is sent
}

extern "C" void UPDI_Idle()
{
    uint32_t baudrate = 100000;
    uart_get_baudrate(UPDI_uart_num, &baudrate);
    esp_rom_delay_us(15000000/baudrate);
}

extern "C"
{
    void wifi_init_softap()
    {
        esp_netif_create_default_wifi_ap();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));
        ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

        wifi_config_t wifi_config = {
            .ap = {
                .ssid = "EFIGenie",
                // .password = EXAMPLE_ESP_WIFI_PASS,
                .ssid_len = strlen("EFIGenie"),
                .authmode = WIFI_AUTH_WPA2_PSK,
                .max_connection = 5,
                .pmf_cfg = { .required = true }
            },
        };
        if (strlen((char *)wifi_config.ap.password) == 0) {
            wifi_config.ap.authmode = WIFI_AUTH_OPEN;
        }

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());
    }

    bool W806_RX_Hook(const uint8_t *data, size_t len)
    {
        if(data && len > 5 && strncmp((const char *)data, "AT+Z\r\n", 6) == 0) {
            xTaskCreate(reset_w806_task, "reset_w806", 4096, 0, 8, NULL);
            return false;
        }
        return true;
    }

    void UPDI1_Enable_Task(void *arg) {
        vTaskDelay(pdMS_TO_TICKS(100));
        UPDI_Enable((uart_port_t)1, UPDI1_UART_TX_PIN, UPDI1_UART_RX_PIN);
        vTaskDelete(NULL);
    }

    void UPDI2_Enable_Task(void *arg) {
        vTaskDelay(pdMS_TO_TICKS(100));
        UPDI_Enable((uart_port_t)1, UPDI1_UART_TX_PIN, UPDI1_UART_RX_PIN);
        vTaskDelete(NULL);
    }

    uint8_t updi_enabled = 0;
    bool UPDI1_RX_Hook(const uint8_t *data, size_t len)
    {
        if(updi_enabled != 1) {
            xTaskCreate(UPDI1_Enable_Task, "UPDI1_Enable_Task", 4096, 0, 5, NULL);
        }
        updi_enabled = 1;
        return true;
    }

    bool UPDI2_RX_Hook(const uint8_t *data, size_t len)
    {
        if(updi_enabled != 2) {
            xTaskCreate(UPDI2_Enable_Task, "UPDI2_Enable_Task", 4096, 0, 5, NULL);
        }
        updi_enabled = 2;
        return true;
    }

    void *_config = 0;
    GeneratorMap<Variable> *_variableMap;
    EmbeddedIOServiceCollection _embeddedIOServiceCollection;
    ICommunicationService *_communicationService;
    CommunicationHandler_EFIGenie *_efiGenieHandler;
    ExpanderMain *_expanderMain;
    Variable *loopTime;
    uint32_t prev;

    bool loadConfig()
    {
        const char * configPath = "/SPIFFS/config.bin";
        FILE *fd = NULL;
        struct stat file_stat;

        if(stat(configPath, &file_stat) == -1)
            return false;
        fd = fopen(configPath, "r");
        if(!fd)
            return false;
        free(_config);
        _config = malloc(file_stat.st_size);
        fread(_config, 1, file_stat.st_size, fd);
        fclose(fd);
        return true;
    }

    bool expandermain_write(void *destination, const void *data, size_t length) {
        if(reinterpret_cast<size_t>(destination) >= 0x20000000 && reinterpret_cast<size_t>(destination) <= 0x2000FA00)
        {
            std::memcpy(destination, data, length);
        }
        else if(reinterpret_cast<size_t>(destination) >= 0x8004000 && reinterpret_cast<size_t>(destination) <= 0x8008000)
        {
            //TODO Write to flash
        }

        return true;
    }

    bool expandermain_quit() {
        if(_expanderMain != 0)
        {
            delete _expanderMain;
            _expanderMain = 0;
        }
        return true;
    }

    bool expandermain_start() {
        if(_expanderMain == 0)
        {
            if(!loadConfig())
                return false;

            size_t configSize = 0;
            _expanderMain = new ExpanderMain(&_config, configSize, &_embeddedIOServiceCollection, _variableMap);

            _expanderMain->Setup();
        }
        return true;
    }

    void Setup() 
    {
        if(!loadConfig())
            return;
        if(_config == 0)
            return;
        _variableMap = new GeneratorMap<Variable>();

        size_t _configSize = 0;
        _expanderMain = new ExpanderMain(reinterpret_cast<void*>(_config), _configSize, &_embeddedIOServiceCollection, _variableMap);

        _efiGenieHandler = new CommunicationHandler_EFIGenie(_variableMap, expandermain_write, expandermain_quit, expandermain_start, _config);
        // ESP_LOGI("ASDF", "_config %p ", _efiGenieHandler->_config);
        _communicationService->RegisterReceiveCallBack([](communication_send_callback_t send, const void *data, size_t length){ return _efiGenieHandler->Receive(send, data, length);});

        _expanderMain->Setup();
        loopTime = _variableMap->GenerateValue(250);
    }
    void Loop() 
    {
        if(_expanderMain != 0) {
            const tick_t now = _embeddedIOServiceCollection.TimerService->GetTick();
            *loopTime = (float)(now-prev) / _embeddedIOServiceCollection.TimerService->GetTicksPerSecond();
            prev = now;
            _expanderMain->Loop();
        }
    }

    Esp32IdfAnalogService *_esp32AnalogService;
    Esp32IdfDigitalService *_esp32DigitalService;
    Esp32IdfPwmService *_esp32PwmService;

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
        io_conf.pull_down_en = (gpio_pulldown_t)0;
        io_conf.pull_up_en = (gpio_pullup_t)0;
        ESP_ERROR_CHECK(gpio_config(&io_conf));
        bootset_w806();
        reset_w806();

        //install uart listen services
        uart_listen_config_t uart_listen_config[UART_NUM_MAX];
        char uart_listen_name[UART_NUM_MAX][16];
        for(uint8_t i = UART_NUM_0; i < UART_NUM_MAX; i++)
        {
            uart_listen_config[i].uart_num = (uart_port_t)i;
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
            .uart_num = (uart_port_t)0,
            .uart_config = &W806_uart_config,
            .tx_pin = W806_UART_TX_PIN,
            .rx_pin = W806_UART_RX_PIN,
            .sock_rx_hook = W806_RX_Hook
        };

        xTaskCreate(sock_uart, "w806_sock_uart", 4096, &W806_sock_uart_config, 5, NULL);

        uart_config_t UPDI_uart_config = {
            .baud_rate = 100000,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_EVEN,
            .stop_bits = UART_STOP_BITS_2, 
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_DEFAULT,
        };

        sock_uart_config_t UPDI1_sock_uart_config = 
        {
            .port = 8001,
            .sock_rx_buffer_size = 1440,
            .uart_num = (uart_port_t)1,
            .uart_config = &UPDI_uart_config,
            .tx_pin = UPDI1_UART_TX_PIN,
            .rx_pin = UPDI1_UART_RX_PIN,
            .sock_rx_hook = UPDI1_RX_Hook
        };

        xTaskCreate(sock_uart, "UPDI1_sock_uart", 4096, &UPDI1_sock_uart_config, 5, NULL);

        sock_uart_config_t UPDI2_sock_uart_config = 
        {
            .port = 8002,
            .sock_rx_buffer_size = 1440,
            .uart_num = (uart_port_t)1,
            .uart_config = &UPDI_uart_config,
            .tx_pin = UPDI2_UART_TX_PIN,
            .rx_pin = UPDI2_UART_RX_PIN,
            .sock_rx_hook = UPDI2_RX_Hook
        };

        xTaskCreate(sock_uart, "UPDI2_sock_uart", 4096, &UPDI2_sock_uart_config, 5, NULL);

        // mount_sd("/SD");
        start_http_server();

        _embeddedIOServiceCollection.AnalogService = new Esp32IdfAnalogService();
        _embeddedIOServiceCollection.DigitalService  = new Esp32IdfDigitalService();
        _embeddedIOServiceCollection.PwmService = new Esp32IdfPwmService();
        _embeddedIOServiceCollection.TimerService = new Esp32IdfTimerService();

        const Esp32IdfCANServiceChannelConfig canconfigs[SOC_TWAI_CONTROLLER_NUM] 
        {
            {
                .enabled = true,
                .t_config = TWAI_TIMING_CONFIG_500KBITS(),
                .f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL(),
                .g_config = TWAI_GENERAL_CONFIG_DEFAULT_V2(0, (gpio_num_t)9, (gpio_num_t)8, TWAI_MODE_NORMAL)
            }
        };
        _embeddedIOServiceCollection.CANService = new Esp32IdfCANService(canconfigs);
        _communicationService = new Esp32IdfCommunicationService_WebSocket(server, "/EFIGenieCommunication");

		const httpd_uri_t resetPost = {
            .uri       = "/command/reset",
			.method     = HTTP_POST,
			.handler    = [](httpd_req_t *req) 
			{
                esp_restart();

				return ESP_OK;
			}
		};

        httpd_register_uri_handler(server, &resetPost);

        mount_spiffs("/SPIFFS");
        register_file_handler_http_server("/SPIFFS");

        Setup();
        while (1)
        {          
            vTaskDelay(1);

            Loop();
        }
    }
}