#include <stdio.h>
#include <string.h>  // For memset
#include <inttypes.h>  // For PRId64
#include "driver/spi_slave.h"
#include "driver/gpio.h"  // For gpio_get_level
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"  // For esp_timer_get_time

#define TAG "SPI_SLAVE"

// Define GPIO pins for SPI
#define PIN_NUM_MOSI 6   // Use GPIO6 for MOSI
#define PIN_NUM_CLK  18  // Use GPIO18 for SCLK
#define PIN_NUM_CS   5   // Use GPIO5 for CS

void app_main(void) {
    // SPI bus configuration
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = -1,  // No MISO line needed
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 16  // 16 bits = 2 bytes
    };

    // SPI slave interface configuration
    spi_slave_interface_config_t slvcfg = {
        .mode = 0,  // SPI mode 0
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 3,
        .flags = 0,
        .post_setup_cb = NULL,
        .post_trans_cb = NULL
    };

    // Initialize SPI slave interface
    ESP_ERROR_CHECK(spi_slave_initialize(SPI2_HOST, &buscfg, &slvcfg, SPI_DMA_DISABLED));

    // Buffer to hold received data
    uint8_t recvbuf[2] = {0};  // 16-bit data = 2 bytes

    // Transaction descriptor
    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));  // Clear the transaction descriptor
    t.length = 16;  // Transaction length in bits
    t.rx_buffer = recvbuf;

    while (1) {
        // Queue a transaction and wait for it to complete
        ESP_ERROR_CHECK(spi_slave_transmit(SPI2_HOST, &t, portMAX_DELAY));
  
        // Combine the two bytes into a single 16-bit integer
        uint16_t received_value = (recvbuf[0] << 8) | recvbuf[1];
      
        // Log the received decimal value
        ESP_LOGI(TAG, "Received decimal value: %u", received_value);
    }
}