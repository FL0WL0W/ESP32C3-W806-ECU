
#include "sock_uart.h"
#include "uart_listen.h"
#include "lwip/sockets.h"
#include "esp_log.h"

typedef struct
{
    sock_uart_config_t *sock_uart_config;
    int sock;
} sock_uart_read_config_t;

#define W806_RESET_PIN 10
static void reset_w806(void *arg)
{
    ESP_LOGI("W806", "Resetting");
    ESP_ERROR_CHECK(gpio_set_level(static_cast<gpio_num_t>(W806_RESET_PIN), 0));
    vTaskDelay(pdMS_TO_TICKS(1));
    ESP_ERROR_CHECK(gpio_set_level(static_cast<gpio_num_t>(W806_RESET_PIN), 1));
    vTaskDelete(NULL);
}

void sock_uart_read(void *arg)
{
    sock_uart_read_config_t *config = (sock_uart_read_config_t *)arg;

    while(!uart_is_driver_installed(config->sock_uart_config->uart_num)) vTaskDelay(pdMS_TO_TICKS(100));//wait for uart driver to be installed by uart_listen
    ESP_ERROR_CHECK(uart_param_config(config->sock_uart_config->uart_num, config->sock_uart_config->uart_config));
    ESP_ERROR_CHECK(uart_set_pin(config->sock_uart_config->uart_num, config->sock_uart_config->rx_pin, config->sock_uart_config->tx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    auto uart_callback_iterator = uart_listen_add_callback(config->sock_uart_config->uart_num, [&](const uint8_t *rx_buffer, size_t len) {
        send(config->sock, rx_buffer, len, 0);
    });

    int len;
    uint8_t rx_buffer[config->sock_uart_config->sock_rx_buffer_size];
    while(1)
    {
        len = recv(config->sock, rx_buffer, sizeof(rx_buffer), 0);
        if (len == 0 || len == -1) {
            ESP_LOGW("SOCK_UART", "Connection closed");
            goto sock_uart_read_cleanup;
        } else if (len < 0) {
            ESP_LOGE("SOCK_UART", "Error occurred during receiving: errno %d", len);
        } else {
            if(strncmp((const char *)rx_buffer, "AT+Z\r\n", 6) == 0) {
                xTaskCreate(reset_w806, "reset_w806", 4096, arg, 8, NULL);
            } else {
                uart_write_bytes(config->sock_uart_config->uart_num, rx_buffer, len);
            }
        }
    }

sock_uart_read_cleanup:
    uart_listen_remove_callback(config->sock_uart_config->uart_num, uart_callback_iterator);
    shutdown(config->sock, 0);
    close(config->sock);
    vTaskDelete(NULL);
}

extern "C" {
    void sock_uart(void *arg)
    {
        sock_uart_config_t *config = (sock_uart_config_t *)arg;

        int keepAlive = 1;
        int keepIdle = 5;
        int keepInterval = 5;
        int keepCount = 3;
        int noDelay = 1;
        int tos = IPTOS_LOWDELAY;
        struct sockaddr_storage dest_addr;

        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(config->port);

        int listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (listen_sock < 0) 
        {
            ESP_LOGE("SOCK_UART", "Unable to create socket: errno %d", errno);
            vTaskDelete(NULL);
            return;
        }
        int opt = 1;
        setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) 
        {
            ESP_LOGE("SOCK_UART", "Socket unable to bind: errno %d", errno);
            goto sock_uart_cleanup;
        }

        err = listen(listen_sock, 1);
        if (err != 0) 
        {
            ESP_LOGE("SOCK_UART", "Error occurred during listen: errno %d", errno);
            goto sock_uart_cleanup;
        }

        while (1) 
        {
            struct sockaddr_storage source_addr;
            socklen_t addr_len = sizeof(source_addr);
            int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
            if (sock < 0) {
                ESP_LOGE("SOCK_UART", "Unable to accept connection: errno %d", errno);
                continue;
            }

            // Set tcp keepalive option
            setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
            setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
            setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
            setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
            // set no delay
            setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &noDelay, sizeof(int));
            setsockopt(sock, IPPROTO_IP, IP_TOS, &tos, sizeof(int));

            sock_uart_read_config_t sock_uart_read_config = 
            {
                .sock_uart_config = config,
                .sock = sock
            };
            xTaskCreate(sock_uart_read, "sock_uart_read", 4096, &sock_uart_read_config, 10, NULL);
        }

sock_uart_cleanup:
        close(listen_sock);
        vTaskDelete(NULL);
    }
}