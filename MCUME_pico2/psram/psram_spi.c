#include "iopins.h"

#ifdef HAS_PSRAM
/******************************************************************************

rp2040-psram

Copyright © 2023 Ian Scott

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************/
#include "psram_spi.h"

#include <stdio.h>


#if defined(PSRAM_ASYNC) && defined(PSRAM_ASYNC_SYNCHRONIZE)
void __isr psram_dma_complete_handler() {
#if PSRAM_ASYNC_DMA_IRQ == 0
    dma_hw->ints0 = 1u << async_spi_inst->async_dma_chan;
#elif PSRAM_ASYNC_DMA_IRQ == 1
    dma_hw->ints1 = 1u << async_spi_inst->async_dma_chan;
#else
#error "PSRAM_ASYNC defined without PSRAM_ASYNC_DMA_IRQ set to 0 or 1"
#endif
    /* putchar('@'); */
#if defined(PSRAM_MUTEX)
    mutex_exit(&async_spi_inst->mtx);
#elif defined(PSRAM_SPINLOCK)
    spin_unlock(async_spi_inst->spinlock, async_spi_inst->spin_irq_state);
#endif
}
#endif // defined(PSRAM_ASYNC) && defined(PSRAM_ASYNC_SYNCHRONIZE)

psram_spi_inst_t psram_spi_init_clkdiv(PIO pio, int sm, float clkdiv, bool fudge) {
    psram_spi_inst_t spi;
    spi.pio = pio;
    spi.offset = pio_add_program(spi.pio, fudge ? &spi_psram_fudge_program : &spi_psram_program);
    if (sm == -1) {
        spi.sm = pio_claim_unused_sm(spi.pio, true);
    } else {
        spi.sm = sm;
    }
#if defined(PSRAM_MUTEX)
    mutex_init(&spi.mtx);
#elif defined(PSRAM_SPINLOCK)
    int spin_id = spin_lock_claim_unused(true);
    spi.spinlock = spin_lock_init(spin_id);
#endif
    //gpio_set_drive_strength(PSRAM_PIN_CS, GPIO_DRIVE_STRENGTH_4MA);
    //gpio_set_drive_strength(PSRAM_PIN_SCK, GPIO_DRIVE_STRENGTH_4MA);
    //gpio_set_drive_strength(PSRAM_PIN_MOSI, GPIO_DRIVE_STRENGTH_4MA);
    
    gpio_init(PSRAM_PIN_SCK);
    gpio_pull_up(PSRAM_PIN_SCK);
    gpio_init(PSRAM_PIN_MISO);
    gpio_pull_up(PSRAM_PIN_MISO);
    gpio_init(PSRAM_PIN_MOSI);
    gpio_pull_up(PSRAM_PIN_MOSI);
    gpio_init(PSRAM_PIN_CS);
    gpio_pull_up(PSRAM_PIN_CS);

    /* gpio_set_slew_rate(PSRAM_PIN_CS, GPIO_SLEW_RATE_FAST); */
    /* gpio_set_slew_rate(PSRAM_PIN_SCK, GPIO_SLEW_RATE_FAST); */
    /* gpio_set_slew_rate(PSRAM_PIN_MOSI, GPIO_SLEW_RATE_FAST); */

    pio_spi_psram_cs_init(spi.pio, spi.sm, spi.offset, 8 /*n_bits*/, clkdiv, fudge, PSRAM_PIN_CS, PSRAM_PIN_MOSI, PSRAM_PIN_MISO);

    // Write DMA channel setup
    spi.write_dma_chan = PSR_DMA_CHANNEL; //dma_claim_unused_channel(true);
    spi.write_dma_chan_config = dma_channel_get_default_config(spi.write_dma_chan);
    channel_config_set_transfer_data_size(&spi.write_dma_chan_config, DMA_SIZE_8);
    channel_config_set_read_increment(&spi.write_dma_chan_config, true);
    channel_config_set_write_increment(&spi.write_dma_chan_config, false);
    channel_config_set_dreq(&spi.write_dma_chan_config, pio_get_dreq(spi.pio, spi.sm, true));
    dma_channel_set_write_addr(spi.write_dma_chan, &spi.pio->txf[spi.sm], false);
    dma_channel_set_config(spi.write_dma_chan, &spi.write_dma_chan_config, false);

    // Read DMA channel setup
    spi.read_dma_chan = PSR_DMA_CHANNEL+1; //dma_claim_unused_channel(true);
    spi.read_dma_chan_config = dma_channel_get_default_config(spi.read_dma_chan);
    channel_config_set_transfer_data_size(&spi.read_dma_chan_config, DMA_SIZE_8);
    channel_config_set_read_increment(&spi.read_dma_chan_config, false);
    channel_config_set_write_increment(&spi.read_dma_chan_config, true);
    channel_config_set_dreq(&spi.read_dma_chan_config, pio_get_dreq(spi.pio, spi.sm, false));
    dma_channel_set_read_addr(spi.read_dma_chan, &spi.pio->rxf[spi.sm], false);
    dma_channel_set_config(spi.read_dma_chan, &spi.read_dma_chan_config, false);

#if defined(PSRAM_ASYNC)
    // Asynchronous DMA channel setup
    spi.async_dma_chan = dma_claim_unused_channel(true);
    spi.async_dma_chan_config = dma_channel_get_default_config(spi.async_dma_chan);
    channel_config_set_transfer_data_size(&spi.async_dma_chan_config, DMA_SIZE_8);
    channel_config_set_read_increment(&spi.async_dma_chan_config, true);
    channel_config_set_write_increment(&spi.async_dma_chan_config, false);
    channel_config_set_dreq(&spi.async_dma_chan_config, pio_get_dreq(spi.pio, spi.sm, true));
    dma_channel_set_write_addr(spi.async_dma_chan, &spi.pio->txf[spi.sm], false);
    dma_channel_set_config(spi.async_dma_chan, &spi.async_dma_chan_config, false);

#if defined(PSRAM_ASYNC_COMPLETE)
    irq_set_exclusive_handler(DMA_IRQ_0 + PSRAM_ASYNC_DMA_IRQ, psram_dma_complete_handler);
    dma_irqn_set_channel_enabled(PSRAM_ASYNC_DMA_IRQ, spi.async_dma_chan, true);
    irq_set_enabled(DMA_IRQ_0 + PSRAM_ASYNC_DMA_IRQ, true);
#endif // defined(PSRAM_ASYNC_COMPLETE)
#endif // defined(PSRAM_ASYNC)

    uint8_t psram_reset_en_cmd[] = {
        8,      // 8 bits to write
        0,      // 0 bits to read
        0x66u   // Reset enable command
    };
    pio_spi_write_read_dma_blocking(&spi, psram_reset_en_cmd, 3, 0, 0);
    busy_wait_us(50);
    uint8_t psram_reset_cmd[] = {
        8,      // 8 bits to write
        0,      // 0 bits to read
        0x99u   // Reset command
    };
    pio_spi_write_read_dma_blocking(&spi, psram_reset_cmd, 3, 0, 0);
    busy_wait_us(100);
    
    return spi;
};

psram_spi_inst_t psram_spi_init(PIO pio, int sm) {
    return psram_spi_init_clkdiv(pio, sm, 1.0, true);
}

void psram_spi_uninit(psram_spi_inst_t spi, bool fudge) {
#if defined(PSRAM_ASYNC)
    // Asynchronous DMA channel teardown
    dma_channel_unclaim(spi.async_dma_chan);
#if defined(PSRAM_ASYNC_COMPLETE)
    irq_set_enabled(DMA_IRQ_0 + PSRAM_ASYNC_DMA_IRQ, false);
    dma_irqn_set_channel_enabled(PSRAM_ASYNC_DMA_IRQ, spi.async_dma_chan, false);
    irq_remove_handler(DMA_IRQ_0 + PSRAM_ASYNC_DMA_IRQ, psram_dma_complete_handler);
#endif // defined(PSRAM_ASYNC_COMPLETE)
#endif // defined(PSRAM_ASYNC)

    // Write DMA channel teardown
    dma_channel_unclaim(spi.write_dma_chan);

    // Read DMA channel teardown
    dma_channel_unclaim(spi.read_dma_chan);

#if defined(PSRAM_SPINLOCK)
    int spin_id = spin_lock_get_num(spi.spinlock);
    spin_lock_unclaim(spin_id);
#endif

    pio_sm_unclaim(spi.pio, spi.sm);
    pio_remove_program(spi.pio, fudge ? &spi_psram_fudge_program : &spi_psram_program, spi.offset);
}

int test_psram(psram_spi_inst_t* psram_spi, int increment) {
    puts("Writing PSRAM...");
    uint8_t deadbeef[8] = {0xd, 0xe, 0xa, 0xd, 0xb, 0xe, 0xe, 0xf};
    /* uncomment to write 8 bits at a time. the below for loop for 32-bit writes is much faster
    for (uint32_t addr = 0; addr < (1024 * 1024); ++addr) {
        psram_write8(psram_spi, addr, (addr & 0xFF));
        // psram_write8_async(psram_spi, addr, (addr & 0xFF));
    }
    */
    for (uint32_t addr = 0; addr < (1024 * 1024); addr += 4) {
        uint32_t value = (uint32_t)(
            (((addr + 3) & 0xFF) << 24) |
            (((addr + 2) & 0xFF) << 16) |
            (((addr + 1) & 0xFF) << 8)  |
            (addr & 0XFF));
        psram_write32(psram_spi, addr, value);
    }
    puts("Reading PSRAM...");
    uint32_t psram_begin = time_us_32();
    for (uint32_t addr = 0; addr < (1024 * 1024); addr += increment) {
        uint8_t result = psram_read8(psram_spi, addr);
        uint8_t test = (uint8_t)(addr & 0xFF);
        if (test != result) {
            printf("\nPSRAM failure at address %x (%x != %x)\n", addr, test, result);
            return 1;
        }
    }
    uint32_t psram_elapsed = time_us_32() - psram_begin;
    float psram_speed = 1000000.0 * 1024.0 * 1024 / psram_elapsed / increment;
    printf("8 bit: PSRAM read in %d us, %d B/s (target 705600 B/s)\n", psram_elapsed, (uint32_t)psram_speed);

    psram_begin = time_us_32();
    for (uint32_t addr = 0; addr < (1024 * 1024); addr += (2 * increment)) {
        uint16_t result = psram_read16(psram_spi, addr);
        uint16_t test = (uint16_t)(
            (((addr + 1) & 0xFF) << 8) |
            (addr & 0XFF));
        if (test != result
        ) {
            printf("PSRAM failure at address %x (%x != %x) ", addr, test, result);
            return 1;
        }
    }
    psram_elapsed = (time_us_32() - psram_begin);
    psram_speed = 1000000.0 * 1024 * 1024 / psram_elapsed / increment;
    printf("16 bit: PSRAM read in %d us, %d B/s (target 1411200 B/s)\n", psram_elapsed, (uint32_t)psram_speed);

    psram_begin = time_us_32();
    for (uint32_t addr = 0; addr < (1024 * 1024); addr += (4 * increment)) {
        uint32_t result = psram_read32(psram_spi, addr);
        uint32_t test = (uint32_t)(
            (((addr + 3) & 0xFF) << 24) |
            (((addr + 2) & 0xFF) << 16) |
            (((addr + 1) & 0xFF) << 8)  |
            (addr & 0XFF));
        if (test != result
        ) {
            printf("PSRAM failure at address %x (%x != %x) ", addr, test, result);
            return 1;
        }
    }
    psram_elapsed = (time_us_32() - psram_begin);
    psram_speed = 1000000.0 * 1024 * 1024 / psram_elapsed / increment;
    printf("32 bit: PSRAM read in %d us, %d B/s (target 1411200 B/s)\n", psram_elapsed, (uint32_t)psram_speed);
    return 0;
}
#endif