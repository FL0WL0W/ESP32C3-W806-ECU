
#include "uart_listen.h"

std::list<uart_callback_t> uart_callback_list[UART_NUM_MAX];
extern "C" {
    void uart_listen(void *arg)
    {
        uart_listen_config_t *config = (uart_listen_config_t *)arg;

        if(!uart_is_driver_installed(config->uart_num))
            ESP_ERROR_CHECK(uart_driver_install(config->uart_num, config->rx_buffer_size, config->tx_buffer_size, 0, NULL, 0));
        ESP_ERROR_CHECK(uart_set_rx_full_threshold(config->uart_num, 1));

        uint8_t rx_buffer[config->rx_buffer_size];
        while (1) 
        {
            // Read data from the UART
            size_t len = uart_read_bytes(config->uart_num, rx_buffer, sizeof(rx_buffer), pdMS_TO_TICKS(100));
            // Write data to functions
            if (len) 
            {
                const std::list<uart_callback_t>::iterator begin = uart_callback_list[config->uart_num].begin();
                const std::list<uart_callback_t>::iterator end = uart_callback_list[config->uart_num].end();
                std::list<uart_callback_t>::iterator next = begin;
                while(next != end)
                {
                    (*next)(rx_buffer, len);
                    next++;
                }
            }
        }
    }
}

std::list<uart_callback_t>::iterator uart_listen_add_callback(uart_port_t uart_num, uart_callback_t callback)
{
    return uart_callback_list[uart_num].insert(uart_callback_list[uart_num].begin(), callback);
}
void uart_listen_remove_callback(uart_port_t uart_num, std::list<uart_callback_t>::iterator callback_iterator)
{
    uart_callback_list[uart_num].erase(callback_iterator);
}