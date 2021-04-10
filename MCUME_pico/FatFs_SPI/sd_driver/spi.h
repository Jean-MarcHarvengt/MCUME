/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
 */
#ifndef _SPI_H_
#define _SPI_H_

#include <stdbool.h>
//
// Pico includes
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/spi.h"
#include "pico/types.h"
#include "pico/sem.h"

#define SPI_FILL_CHAR (0xFF)
#define XFER_BLOCK_SIZE 512  // Block size supported for SD card is 512 bytes

// "Class" representing SPIs
typedef struct {
    // SPI HW
    spi_inst_t *hw_inst;
    const uint miso_gpio;  // SPI MISO pin number for GPIO
    const uint mosi_gpio;
    const uint sck_gpio;
    const uint baud_rate;
    // State variables:
    uint tx_dma;
    uint rx_dma;
    dma_channel_config tx_dma_cfg;
    dma_channel_config rx_dma_cfg;
    irq_handler_t dma_isr;
    bool initialized;         // Assigned dynamically
    semaphore_t sem;
} spi_t;

// SPI DMA interrupts
void spi_irq_handler(spi_t *this);

bool spi_transfer(spi_t *this, const uint8_t *tx, uint8_t *rx, size_t length);
bool my_spi_init(spi_t *this);

#define USE_LED 1
#if USE_LED
#   define LED_PIN 25
#   include "hardware/gpio.h"
#   define LED_INIT()                       \
    {                                    \
        gpio_init(LED_PIN);              \
        gpio_set_dir(LED_PIN, GPIO_OUT); \
    }
#   define LED_ON() gpio_put(LED_PIN, 1)
#   define LED_OFF() gpio_put(LED_PIN, 0)
#else
#   define LED_ON()
#   define LED_OFF()
#   define LED_INIT()
#endif

#endif
/* [] END OF FILE */
