#pragma once

#include "hardware/spi.h"
#include "hardware/clocks.h"

#define CLK_SLOW_DEFAULT    (100 * KHZ)
#define CLK_FAST_DEFAULT    (50 * MHZ)
// CLK_FAST: actually set to clk_peri (= 125.0 MHz) / N,
// which is determined by spi_set_baudrate() in pico-sdk/src/rp2_common/hardware_spi/spi.c

/* SPI pin assignment */
#define PIN_SPI0_MISO_DEFAULT   4   // 0, 4, 16
#define PIN_SPI0_CS_DEFAULT     5   // 1, 5, 17
#define PIN_SPI0_SCK_DEFAULT    6   // 2, 6, 18
#define PIN_SPI0_MOSI_DEFAULT   7   // 3, 7, 19

#define PIN_SPI1_MISO_DEFAULT   8   //  8, 12
#define PIN_SPI1_CS_DEFAULT     9   //  9, 13
#define PIN_SPI1_SCK_DEFAULT    10  // 10, 14
#define PIN_SPI1_MOSI_DEFAULT   11  // 11, 15

typedef struct _pico_fatfs_spi_config_t {
    spi_inst_t  *spi_inst;  // spi0 or spi1
    uint        clk_slow;
    uint        clk_fast;
    uint        pin_miso;
    uint        pin_cs;
    uint        pin_sck;
    uint        pin_mosi;
    bool        pullup;     // miso, mosi pins only
} pico_fatfs_spi_config_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
* Set configuration
*
* @param[in] config the pointer of pico_fatfs_spi_config_t to configure with
*/
void pico_fatfs_set_config(pico_fatfs_spi_config_t *config);
int pico_fatfs_reboot_spi(void);

#ifdef __cplusplus
}
#endif