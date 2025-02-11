#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include "http_server.h"
#include "ATTiny_UPDI.h"
#include "w806.h"
#include "freertos/ringbuf.h"
#include "uart_listen.h"

#include "esp_log.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"

#define UPDI1_UART_RX_PIN (gpio_num_t)20
#define UPDI1_UART_TX_PIN (gpio_num_t)19
#define UPDI2_UART_RX_PIN (gpio_num_t)21
#define UPDI2_UART_TX_PIN (gpio_num_t)22

/* Max length a file path can have on storage */
#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + CONFIG_SPIFFS_OBJ_NAME_LEN)

/* Max size of an individual file. Make sure this
 * value is same as that set in upload_script.html */
#define MAX_FILE_SIZE   (200*1024) // 200 KB
#define MAX_FILE_SIZE_STR "200KB"

/* Scratch buffer size */
#define SCRATCH_BUFSIZE  8192
#define ATTINY_FLASH_SIZE 4096
#define ATTINY_FLASH_SIZE_STR "4KB"
#define W806_FLASH_SIZE 1073741824
#define W806_FLASH_SIZE_STR "1MB"

struct http_server_data {
    /* Base path of file storage */
    char base_path[ESP_VFS_PATH_MAX + 1];

    /* Scratch buffer for temporary storage during file transfer */
    char scratch[SCRATCH_BUFSIZE];
};

static const char *TAG = "HTTP_SERVER";

#define IS_FILE_EXT(filename, ext) \
    (strcasecmp(&filename[strlen(filename) - sizeof(ext) + 1], ext) == 0)

/* Set HTTP response content type according to file extension */
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filename)
{
    if (IS_FILE_EXT(filename, ".pdf")) {
        return httpd_resp_set_type(req, "application/pdf");
    } else if (IS_FILE_EXT(filename, ".bin")) {
        return httpd_resp_set_type(req, "application/octet-stream");
    } else if (IS_FILE_EXT(filename, ".html")) {
        return httpd_resp_set_type(req, "text/html; charset=\"UTF-8\"");
    } else if (IS_FILE_EXT(filename, ".css")) {
        return httpd_resp_set_type(req, "text/css; charset=\"UTF-8\"");
    } else if (IS_FILE_EXT(filename, ".js")) {
        return httpd_resp_set_type(req, "text/javascript; charset=\"UTF-8\"");
    } else if (IS_FILE_EXT(filename, ".jpeg")) {
        return httpd_resp_set_type(req, "image/jpeg");
    } else if (IS_FILE_EXT(filename, ".svg")) {
        return httpd_resp_set_type(req, "image/svg+xml");
    } else if (IS_FILE_EXT(filename, ".ico")) {
        return httpd_resp_set_type(req, "image/x-icon");
    }
    /* This is a limited set only */
    /* For any other type always set as plain text */
    return httpd_resp_set_type(req, "text/plain");
}

/* Copies the full path into destination buffer and returns
 * pointer to path (skipping the preceding base path) */
static const char* get_path_from_uri(char *dest, const char *base_path, const char *uri, size_t destsize)
{
    const size_t base_pathlen = strlen(base_path);
    size_t pathlen = strlen(uri);

    const char *quest = strchr(uri, '?');
    if (quest) {
        pathlen = MIN(pathlen, quest - uri);
    }
    const char *hash = strchr(uri, '#');
    if (hash) {
        pathlen = MIN(pathlen, hash - uri);
    }

    if(pathlen < 2) {
        uri = "/index.html";
        pathlen = strlen(uri);
    }

    if (base_pathlen + pathlen + 1 > destsize) {
        /* Full path string won't fit into destination buffer */
        return NULL;
    }

    /* Construct full path (base + path) */
    strcpy(dest, base_path);
    strlcpy(dest + base_pathlen, uri, pathlen + 1);

    /* Return pointer to path, skipping the base */
    return dest + base_pathlen;
}

/* Handler to download a file kept on the server */
static esp_err_t download_get_handler(httpd_req_t *req)
{
    char filepath[FILE_PATH_MAX];
    FILE *fd = NULL;
    struct stat file_stat;

    const char *filename = get_path_from_uri(filepath, ((struct http_server_data *)req->user_ctx)->base_path,
                                             req->uri, sizeof(filepath));

    if (!filename) {
        ESP_LOGE(TAG, "Filename is too long");
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too long\r\n");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filename);
    if (stat(filepath, &file_stat) == -1) {
        httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
        strcat(filepath, ".gz");
    }

    if (stat(filepath, &file_stat) == -1) {
        ESP_LOGE(TAG, "Failed to stat file : %s", filepath);
        /* Respond with 404 Not Found */
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File does not exist\r\n");
        return ESP_FAIL;
    }

    fd = fopen(filepath, "r");
    if (!fd) {
        ESP_LOGE(TAG, "Failed to read existing file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file\r\n");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Sending file : %s (%ld bytes)...", filename, file_stat.st_size);

    /* Retrieve the pointer to scratch buffer for temporary storage */
    char *chunk = ((struct http_server_data *)req->user_ctx)->scratch;
    size_t chunksize;
    do {
        /* Read file in chunks into the scratch buffer */
        chunksize = fread(chunk, 1, SCRATCH_BUFSIZE, fd);

        if (chunksize > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, chunksize) != ESP_OK) {
                fclose(fd);
                ESP_LOGE(TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file\r\n");
               return ESP_FAIL;
           }
        }

        /* Keep looping till the whole file is sent */
    } while (chunksize != 0);

    /* Close file after sending complete */
    fclose(fd);
    ESP_LOGI(TAG, "File sending complete");

    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

int64_t get_timestamp() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000000LL + tv.tv_usec);
}

/* Handler to upload flash to W806 */
static esp_err_t upload_w806_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Receiving W806 Flash");
    int retry_num = 0;
    unsigned short crc_value = 0;
    unsigned char ack_id = 0x06;
    int pack_counter = 0;

    /* Retrieve the pointer to scratch buffer for temporary storage */
    char *buf = ((struct http_server_data *)req->user_ctx)->scratch;
    int received;

    uint32_t previousBaudRate;
    ESP_ERROR_CHECK(uart_get_baudrate(W806_UART_NUM, &previousBaudRate));

    /* File cannot be larger than a limit */
    if (req->content_len > W806_FLASH_SIZE) {
        ESP_LOGE(TAG, "File too large : %d bytes", req->content_len);
        /* Respond with 400 Bad Request */
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                            "File size must be less than "
                            W806_FLASH_SIZE_STR "!\r\n");
        /* Return failure to close underlying connection else the
         * incoming file content will keep the socket busy */
        return ESP_FAIL;
    }

    /* File must be bigger than 0 */
    if (!(req->content_len > 0)) {
        ESP_LOGE(TAG, "File too small : %d bytes", req->content_len);
        /* Respond with 400 Bad Request */
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                            "File size must be larger than 0!\r\n");
        /* Return failure to close underlying connection else the
         * incoming file content will keep the socket busy */
        return ESP_FAIL;
    }

    /* Content length of the request gives
     * the size of the file being uploaded */
    int remaining = req->content_len;

    //Create ring buffer
    RingbufHandle_t rxbuf_handle;
    rxbuf_handle = xRingbufferCreate(1028, RINGBUF_TYPE_BYTEBUF);
    if (rxbuf_handle == NULL) {
        ESP_LOGE(TAG, "Failed to create ring buffer!");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to create ring buffer\r\n");
        return ESP_FAIL;
    }

    //reset w806
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_sendstr_chunk(req, "Resetting W806 into flash mode\r\n");
    ESP_ERROR_CHECK(uart_set_pin(W806_UART_NUM, W806_UART_TX_PIN, W806_UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_set_baudrate(W806_UART_NUM, 115200));
    bootreset_w806();
    reset_w806();

    auto rx_listen_callback_iterator = uart_listen_add_callback(W806_UART_NUM, [&](const uint8_t *rx_buffer, size_t len) {
        xRingbufferSend(rxbuf_handle, rx_buffer, len, pdMS_TO_TICKS(500));
    });

    //interrupt startup
    int cnt = 0;
    unsigned char esc_key = 27;
    for (int i = 0; i < 100; i++)
    {
        uart_write_bytes(W806_UART_NUM, &esc_key, 1);
        size_t size = 0;
        char *ch = (char *)xRingbufferReceiveUpTo(rxbuf_handle, &size, pdMS_TO_TICKS(10), 1);
        if (size > 0)
        {
            if (('C' == *ch) || ('P' == *ch))
                cnt++;
            else
                cnt = 0;
            vRingbufferReturnItem(rxbuf_handle, ch);
            break;
        }
    }
    httpd_resp_sendstr_chunk(req, "Syncing\r\n");

    //sync
    int64_t start = get_timestamp(); 
    do
    {
        size_t size = 0;
        char *ch = (char *)xRingbufferReceiveUpTo(rxbuf_handle, &size, pdMS_TO_TICKS(500), 1);

        if (size > 0)
        {
            if (('C' == *ch) || ('P' == *ch))
                cnt++;
            else
                cnt = 0;
            vRingbufferReturnItem(rxbuf_handle, ch);
        }
        else
        {
            cnt = 0;
            //reset w806
            bootreset_w806();
            reset_w806();

            for (int i = 0; i < 100; i++)
            {
                uart_write_bytes(W806_UART_NUM, &esc_key, 1);
                ch = (char *)xRingbufferReceiveUpTo(rxbuf_handle, &size, pdMS_TO_TICKS(10), 1);
                if (size > 0)
                {
                    if (('C' == *ch) || ('P' == *ch))
                        cnt++;
                    else
                        cnt = 0;
                    vRingbufferReturnItem(rxbuf_handle, ch);
                    break;
                }
            }
        }

        if((get_timestamp() - start) > 5000000)
        {
            ESP_LOGE(TAG, "Failed to sync");
            /* Respond with 500 Internal Server Error */
            httpd_resp_sendstr_chunk(req, "Failed to sync\r\n");
            goto upload_w806_post_handler_cleanup;
        }
    } while (cnt < 3);

    httpd_resp_sendstr_chunk(req, "Erasing\r\n");
    uart_write_bytes(W806_UART_NUM, wm_tool_chip_cmd_erase, sizeof(wm_tool_chip_cmd_erase));

    //sync
    start = get_timestamp(); 
    do
    {
        size_t size = 0;
        char *ch = (char *)xRingbufferReceiveUpTo(rxbuf_handle, &size, pdMS_TO_TICKS(500), 1);

        if (size > 0)
        {
            if (('C' == *ch) || ('P' == *ch))
                cnt++;
            else
                cnt = 0;
            vRingbufferReturnItem(rxbuf_handle, ch);
        }
        else
        {
            cnt = 0;
            //reset w806
            bootreset_w806();
            reset_w806();

            for (int i = 0; i < 100; i++)
            {
                uart_write_bytes(W806_UART_NUM, &esc_key, 1);
                ch = (char *)xRingbufferReceiveUpTo(rxbuf_handle, &size, pdMS_TO_TICKS(10), 1);
                if (size > 0)
                {
                    if (('C' == *ch) || ('P' == *ch))
                        cnt++;
                    else
                        cnt = 0;
                    vRingbufferReturnItem(rxbuf_handle, ch);
                    break;
                }
            }
        }

        if((get_timestamp() - start) > 5000000)
        {
            ESP_LOGE(TAG, "Failed to erase");
            /* Respond with 500 Internal Server Error */
            httpd_resp_sendstr_chunk(req, "Failed to erase\r\n");
            goto upload_w806_post_handler_cleanup;
        }
    } while (cnt < 3);

    httpd_resp_sendstr_chunk(req, "Changing baud rate to 2Mhz\r\n");
    //change baud rate to 2M
    uart_write_bytes(W806_UART_NUM, wm_tool_chip_cmd_b2000000, sizeof(wm_tool_chip_cmd_b2000000));
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_ERROR_CHECK(uart_set_baudrate(W806_UART_NUM, 2000000));

    httpd_resp_sendstr_chunk(req, "Verifying connection\r\n");
    //sync
    start = get_timestamp(); 
    do
    {
        size_t size = 0;
        char *ch = (char *)xRingbufferReceiveUpTo(rxbuf_handle, &size, pdMS_TO_TICKS(500), 1);

        if (size > 0)
        {
            if (('C' == *ch) || ('P' == *ch))
                cnt++;
            else
                cnt = 0;
            vRingbufferReturnItem(rxbuf_handle, ch);
        }
        else
        {
            cnt = 0;
            //reset w806
            bootreset_w806();
            reset_w806();

            for (int i = 0; i < 100; i++)
            {
                uart_write_bytes(W806_UART_NUM, &esc_key, 1);
                ch = (char *)xRingbufferReceiveUpTo(rxbuf_handle, &size, pdMS_TO_TICKS(10), 1);
                if (size > 0)
                {
                    if (('C' == *ch) || ('P' == *ch))
                        cnt++;
                    else
                        cnt = 0;
                    vRingbufferReturnItem(rxbuf_handle, ch);
                    break;
                }
            }
        }

        if((get_timestamp() - start) > 5000000)
        {
            ESP_LOGE(TAG, "Failed to verify connection\r\n");
            /* Respond with 500 Internal Server Error */
            httpd_resp_sendstr_chunk(req, "Failed to verify connection\r\n");
            goto upload_w806_post_handler_cleanup;
        }
    } while (cnt < 3);

    httpd_resp_sendstr_chunk(req, "Sending File\r\n");
    while (ack_id != 0x04) {

        ESP_LOGI(TAG, "Remaining size : %d\t\tack_id : %x", remaining, ack_id);
        /* Receive the file part by part into a buffer */
        received = 0;
        if(remaining > 0) {
            if(remaining > 0 && ack_id == 0x06) {
                size_t getLen = MIN(remaining, W806_XMODEM_DATA_SIZE);
                while(received < getLen) {
                    int r;
                    if ((r = httpd_req_recv(req, buf + 3 + received, getLen - received)) <= 0) {
                        if (received == HTTPD_SOCK_ERR_TIMEOUT) {
                            /* Retry if timeout occurred */
                            continue;
                        }

                        ESP_LOGE(TAG, "File reception failed!");
                        /* Respond with 500 Internal Server Error */
                        httpd_resp_sendstr_chunk(req, "Failed to receive file\r\n");
                        goto upload_w806_post_handler_cleanup;
                    }
                    received += r;
                }
            }
        } else {
            received = -1;
        }

        /* Write buffer content to W806 */
        switch(ack_id)
        {
            case 0x06:
            {
                httpd_resp_sendstr_chunk(req, "...\r\n");
                retry_num = 0;
                pack_counter++;

                if (received > 0)
                {
                    /* Keep track of remaining size of
                    * the file left to be uploaded */
                    remaining -= received;
                    if (received < W806_XMODEM_DATA_SIZE)
                    {
                        for ( ; received < W806_XMODEM_DATA_SIZE; received++)
                            buf[received + 3] = 0x0;
                    }

                    buf[0] = 0x02;
                    buf[1] = pack_counter;
                    buf[2] = 255 - buf[1];

                    crc_value = w806_crc16(buf + 3, W806_XMODEM_DATA_SIZE);

                    buf[W806_XMODEM_DATA_SIZE + 3]=(unsigned char)(crc_value >> 8);
                    buf[W806_XMODEM_DATA_SIZE + 4]=(unsigned char)(crc_value);

                    //send frame header
                    uart_write_bytes(W806_UART_NUM, buf, 1);

                    //clear rx buffer
                    size_t size = 0;
                    char *ch = (char *)xRingbufferReceive(rxbuf_handle, &size, 0);
                    if(size > 0)
                        vRingbufferReturnItem(rxbuf_handle, ch);

                    //send frame
                    uart_write_bytes(W806_UART_NUM, buf + 1, W806_XMODEM_DATA_SIZE + 4);

                    //wait for ACK
                    ch = (char *)xRingbufferReceiveUpTo(rxbuf_handle, &size, pdMS_TO_TICKS(500), 1);
                    if(size > 0)
                        ack_id = *ch;
                    else
                        ack_id = 0x15;
                    if(size > 0)
                        vRingbufferReturnItem(rxbuf_handle, ch);
                }
                else
                {
                    ack_id = 0x04;

                    while (ack_id != 0x06)
                    {
                        ack_id = 0x04;
                        //clear rx buffer
                        size_t size = 0;
                        char *ch = (char *)xRingbufferReceive(rxbuf_handle, &size, 0);
                        if(size > 0)
                            vRingbufferReturnItem(rxbuf_handle, ch);

                        //send frame
                        uart_write_bytes(W806_UART_NUM, &ack_id, 1);

                        //wait for ACK
                        ch = (char *)xRingbufferReceiveUpTo(rxbuf_handle, &size, pdMS_TO_TICKS(500), 1);
                        if(size > 0)
                            ack_id = *ch;
                        else
                            ack_id = 0x15;
                        if(size > 0)
                            vRingbufferReturnItem(rxbuf_handle, ch);
                    }
                    ack_id = 0x04;
                }
                break;
            }
            case 0x15:
            {
                httpd_resp_sendstr_chunk(req, "***\r\n");
                if ( retry_num++ > 100)
                {
                    ESP_LOGE(TAG, "retry too many times, quit!");
                    /* Respond with 500 Internal Server Error */
                    httpd_resp_sendstr_chunk(req, "Timed out trying to program flash\r\n");
                    goto upload_w806_post_handler_cleanup;
                }
                else
                {
                    //send frame header
                    uart_write_bytes(W806_UART_NUM, buf, 5);

                    //clear rx buffer
                    size_t size = 0;
                    char *ch = (char *)xRingbufferReceive(rxbuf_handle, &size, 0);
                    if(size > 0)
                        vRingbufferReturnItem(rxbuf_handle, ch);

                    //send frame
                    uart_write_bytes(W806_UART_NUM, buf + 5, W806_XMODEM_DATA_SIZE);

                    //wait for ACK
                    ch = (char *)xRingbufferReceiveUpTo(rxbuf_handle, &size, pdMS_TO_TICKS(500), 1);
                    if(size > 0)
                        ack_id = *ch;
                    else
                        ack_id = 0x15;
                    if(size > 0)
                        vRingbufferReturnItem(rxbuf_handle, ch);
                }
                break;
            }
            case 0x18: //waiting
            case 0x43:
            {
                httpd_resp_sendstr_chunk(req, "___\r\n");
                size_t size = 0;
                char *ch = (char *)xRingbufferReceiveUpTo(rxbuf_handle, &size, pdMS_TO_TICKS(500), 1);
                if(size > 0) {
                    vRingbufferReturnItem(rxbuf_handle, ch);
                    ack_id = ch[size-1];
                    break;
                }
            }
            default:
            {
                ESP_LOGE(TAG, "Program failed! Unkown xmodem protocol [%x].", ack_id);
                /* Respond with 500 Internal Server Error */
                httpd_resp_sendstr_chunk(req, "Failed to program flash. Unkown xmodem protocol.\r\n");
                goto upload_w806_post_handler_cleanup;
            }
        }
    }

    ESP_LOGI(TAG, "File reception complete");

    /* Redirect onto root to see the updated file list */
    httpd_resp_sendstr_chunk(req, "\r\nFlash programmed successfully\r\n");
upload_w806_post_handler_cleanup:
    httpd_resp_set_status(req, "200");
    httpd_resp_send_chunk(req, NULL, 0);
    ESP_ERROR_CHECK(uart_set_baudrate(W806_UART_NUM, previousBaudRate));
    uart_listen_remove_callback(W806_UART_NUM, rx_listen_callback_iterator);
    vRingbufferDelete(rxbuf_handle);
    bootset_w806();
    reset_w806();
    return ESP_OK;
}

/* Handler to upload flash to ATTiny */
static esp_err_t upload_attiny_post_handler(httpd_req_t *req)
{
    /* Skip leading "/upload/attiny" from URI to get refdes */
    /* Note sizeof() counts NULL termination hence the -1 */
    const char *attiny_refdes = req->uri + sizeof("/upload/attiny/") - 1;
    if (!attiny_refdes || (attiny_refdes[0] != '1' && attiny_refdes[0] != '2') || attiny_refdes[1] != 0) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "refdes incorrect\r\n");
        return ESP_FAIL;
    }

    gpio_num_t tx_pin = UPDI1_UART_TX_PIN;
    gpio_num_t rx_pin = UPDI1_UART_RX_PIN;

    if(attiny_refdes[0] == '2' ) {
        tx_pin = UPDI2_UART_TX_PIN;
        rx_pin = UPDI2_UART_RX_PIN;
    }

    ESP_LOGI(TAG, "Receiving Attiny Flash for U%s", attiny_refdes);

    /* Retrieve the pointer to scratch buffer for temporary storage */
    char *buf = ((struct http_server_data *)req->user_ctx)->scratch;
    int received;

    /* File cannot be larger than a limit */
    if (req->content_len > ATTINY_FLASH_SIZE) {
        ESP_LOGE(TAG, "File too large : %d bytes", req->content_len);
        /* Respond with 400 Bad Request */
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                            "File size must be less than "
                            ATTINY_FLASH_SIZE_STR "!\r\n");
        /* Return failure to close underlying connection else the
         * incoming file content will keep the socket busy */
        return ESP_FAIL;
    }

    /* Content length of the request gives
     * the size of the file being uploaded */
    int remaining = req->content_len;

    while (remaining > 0) {

        ESP_LOGI(TAG, "Remaining size : %d", remaining);
        /* Receive the file part by part into a buffer */
        if ((received = httpd_req_recv(req, buf + (req->content_len - remaining), MIN(remaining, SCRATCH_BUFSIZE))) <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry if timeout occurred */
                continue;
            }

            ESP_LOGE(TAG, "File reception failed!");
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive file\r\n");
            return ESP_FAIL;
        }

        /* Keep track of remaining size of
         * the file left to be uploaded */
        remaining -= received;
    }

    httpd_resp_set_type(req, "text/plain");
    httpd_resp_sendstr_chunk(req, "Programming Flash\r\n");
    /* Write buffer content to ATTiny. retry once if failed first time */
    if (!UPDI_Program((uart_port_t)1, tx_pin, rx_pin, (uint8_t *)buf, req->content_len)) {
        httpd_resp_sendstr_chunk(req, "Failed. Retrying...\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        if(!UPDI_Program((uart_port_t)1, tx_pin, rx_pin, (uint8_t *)buf, req->content_len)) {
            ESP_LOGE(TAG, "Program failed!");
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to program flash\r\n");
            return ESP_FAIL;
        }
    }

    ESP_LOGI(TAG, "File reception complete");

    /* Redirect onto root to see the updated file list */
    httpd_resp_sendstr_chunk(req, "Flash programmed successfully\r\n");
    httpd_resp_set_status(req, "200");
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

/* Handler to upload a file onto the server */
static esp_err_t upload_post_handler(httpd_req_t *req)
{
    if(strncmp(req->uri + sizeof("/upload"), "attiny/", sizeof("attiny/") - 1) == 0) {
        return upload_attiny_post_handler(req);
    }

    if(strncmp(req->uri + sizeof("/upload"), "w806", sizeof("w806") - 1) == 0) {
        return upload_w806_post_handler(req);
    }

    char filepath[FILE_PATH_MAX];
    FILE *fd = NULL;
    struct stat file_stat;

    /* Skip leading "/upload" from URI to get filename */
    /* Note sizeof() counts NULL termination hence the -1 */
    const char *filename = get_path_from_uri(filepath, ((struct http_server_data *)req->user_ctx)->base_path,
                                             req->uri + sizeof("/upload") - 1, sizeof(filepath));
    if (!filename) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too longr\r\n");
        return ESP_FAIL;
    }

    /* Filename cannot have a trailing '/' */
    if (filename[strlen(filename) - 1] == '/') {
        ESP_LOGE(TAG, "Invalid filename : %s", filename);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Invalid filename\r\n");
        return ESP_FAIL;
    }

    if (stat(filepath, &file_stat) == 0) {
        ESP_LOGE(TAG, "File already exists : %s", filepath);
        ESP_LOGI(TAG, "Delting : %s", filepath);
        unlink(filepath);
    }

    /* File cannot be larger than a limit */
    if (req->content_len > MAX_FILE_SIZE) {
        ESP_LOGE(TAG, "File too large : %d bytes", req->content_len);
        /* Respond with 400 Bad Request */
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST,
                            "File size must be less than "
                            MAX_FILE_SIZE_STR "!\r\n");
        /* Return failure to close underlying connection else the
         * incoming file content will keep the socket busy */
        return ESP_FAIL;
    }

    fd = fopen(filepath, "w");
    if (!fd) {
        ESP_LOGE(TAG, "Failed to create file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to create file\r\n");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Receiving file : %s...", filename);

    /* Retrieve the pointer to scratch buffer for temporary storage */
    char *buf = ((struct http_server_data *)req->user_ctx)->scratch;
    int received;

    /* Content length of the request gives
     * the size of the file being uploaded */
    int remaining = req->content_len;

    while (remaining > 0) {

        ESP_LOGI(TAG, "Remaining size : %d", remaining);
        /* Receive the file part by part into a buffer */
        if ((received = httpd_req_recv(req, buf, MIN(remaining, SCRATCH_BUFSIZE))) <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry if timeout occurred */
                continue;
            }

            /* In case of unrecoverable error,
             * close and delete the unfinished file*/
            fclose(fd);
            unlink(filepath);

            ESP_LOGE(TAG, "File reception failed!");
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive file\r\n");
            return ESP_FAIL;
        }

        /* Write buffer content to file on storage */
        if (received && (received != fwrite(buf, 1, received, fd))) {
            /* Couldn't write everything to file!
             * Storage may be full? */
            fclose(fd);
            unlink(filepath);

            ESP_LOGE(TAG, "File write failed!");
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to write file to storage\r\n");
            return ESP_FAIL;
        }

        /* Keep track of remaining size of
         * the file left to be uploaded */
        remaining -= received;
    }

    /* Close file upon upload completion */
    fclose(fd);
    ESP_LOGI(TAG, "File reception complete");

    /* Redirect onto root to see the updated file list */
    httpd_resp_set_status(req, "200");
    httpd_resp_sendstr(req, "File uploaded successfully\r\n");
    return ESP_OK;
}

/* Handler to delete a file from the server */
static esp_err_t delete_post_handler(httpd_req_t *req)
{
    char filepath[FILE_PATH_MAX];
    struct stat file_stat;

    /* Skip leading "/delete" from URI to get filename */
    /* Note sizeof() counts NULL termination hence the -1 */
    const char *filename = get_path_from_uri(filepath, ((struct http_server_data *)req->user_ctx)->base_path,
                                             req->uri  + sizeof("/delete") - 1, sizeof(filepath));
    if (!filename) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too long\r\n");
        return ESP_FAIL;
    }

    /* Filename cannot have a trailing '/' */
    if (filename[strlen(filename) - 1] == '/') {
        ESP_LOGE(TAG, "Invalid filename : %s", filename);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Invalid filename\r\n");
        return ESP_FAIL;
    }

    if (stat(filepath, &file_stat) == -1) {
        ESP_LOGE(TAG, "File does not exist : %s", filename);
        /* Respond with 400 Bad Request */
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "File does not exist\r\n");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Deleting file : %s", filename);
    /* Delete file */
    unlink(filepath);

    /* Redirect onto root to see the updated file list */
    httpd_resp_set_status(req, "200");
    httpd_resp_sendstr(req, "File deleted successfully\r\n");
    return ESP_OK;
}

httpd_handle_t server = NULL;
/* Function to start the file server */
esp_err_t start_http_server()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Use the URI wildcard matching function in order to
     * allow the same handler to respond to multiple different
     * target URIs which match the wildcard scheme */
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG, "Starting HTTP Server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server!");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t register_file_handler_http_server(const char *base_path)
{
    static struct http_server_data *server_data = NULL;

    if (server_data) {
        ESP_LOGE(TAG, "File server already started");
        return ESP_ERR_INVALID_STATE;
    }

    /* Allocate memory for server data */
    server_data = (http_server_data *)calloc(1, sizeof(struct http_server_data));
    if (!server_data) {
        ESP_LOGE(TAG, "Failed to allocate memory for server data");
        return ESP_ERR_NO_MEM;
    }
    strlcpy(server_data->base_path, base_path,
            sizeof(server_data->base_path));

    /* URI handler for getting uploaded files */
    httpd_uri_t file_download = {
        .uri       = "/*",  // Match all URIs of type /path/to/file
        .method    = HTTP_GET,
        .handler   = download_get_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &file_download);

    /* URI handler for uploading files to server */
    httpd_uri_t file_upload = {
        .uri       = "/upload/*",   // Match all URIs of type /upload/path/to/file
        .method    = HTTP_POST,
        .handler   = upload_post_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &file_upload);

    /* URI handler for deleting files from server */
    httpd_uri_t file_delete = {
        .uri       = "/delete/*",   // Match all URIs of type /delete/path/to/file
        .method    = HTTP_POST,
        .handler   = delete_post_handler,
        .user_ctx  = server_data    // Pass server data as context
    };
    httpd_register_uri_handler(server, &file_delete);

    return ESP_OK;
}
