#include <stdio.h>
#include <string.h>

#include <pico/stdlib.h>

#include "psram_spi.h"

psram_spi_inst_t* async_spi_inst;

int main()
{
    // Overclock!
    set_sys_clock_khz(280000, true);

    stdio_init_all();
    
    puts("PSRAM test - rp2040-psram v1.0.0");

    puts("Initing PSRAM...");
    psram_spi_inst_t psram_spi = psram_spi_init(pio0, -1);

    uint32_t psram_begin, psram_elapsed;
    float psram_speed;

    puts("Testing PSRAM...");

    // **************** 8 bits testing ****************
    psram_begin = time_us_32();
    for (uint32_t addr = 0; addr < (8 * 1024 * 1024); ++addr) {
        psram_write8(&psram_spi, addr, (addr & 0xFF));
    }
    psram_elapsed = time_us_32() - psram_begin;
    psram_speed = 1000000.0 * 8 * 1024.0 * 1024 / psram_elapsed;
    printf("8 bit: PSRAM write 8MB in %d us, %d B/s\n", psram_elapsed, (uint32_t)psram_speed);

    psram_begin = time_us_32();
    for (uint32_t addr = 0; addr < (8 * 1024 * 1024); ++addr) {
        psram_write8_async(&psram_spi, addr, (addr & 0xFF));
    }
    psram_elapsed = time_us_32() - psram_begin;
    psram_speed = 1000000.0 * 8 * 1024.0 * 1024 / psram_elapsed;
    printf("8 bit: PSRAM write async 8MB in %d us, %d B/s\n", psram_elapsed, (uint32_t)psram_speed);

    psram_begin = time_us_32();
    for (uint32_t addr = 0; addr < (8 * 1024 * 1024); ++addr) {
        uint8_t result = psram_read8(&psram_spi, addr);
        if ((uint8_t)(addr & 0xFF) != result) {
            printf("\nPSRAM failure at address %x (%x != %x)\n", addr, addr & 0xFF, result);
            return 1;
        }
    }
    psram_elapsed = time_us_32() - psram_begin;
    psram_speed = 1000000.0 * 8 * 1024.0 * 1024 / psram_elapsed;
    printf("8 bit: PSRAM read 8MB in %d us, %d B/s\n", psram_elapsed, (uint32_t)psram_speed);

    // **************** 16 bits testing ****************
    psram_begin = time_us_32();
    for (uint32_t addr = 0; addr < (8 * 1024 * 1024); addr += 2) {
        psram_write16(&psram_spi, addr, (((addr + 1) & 0xFF) << 8) | (addr & 0xFF));
    }
    psram_elapsed = time_us_32() - psram_begin;
    psram_speed = 1000000.0 * 8 * 1024.0 * 1024 / psram_elapsed;
    printf("16 bit: PSRAM write 8MB in %d us, %d B/s\n", psram_elapsed, (uint32_t)psram_speed);

    psram_begin = time_us_32();
    for (uint32_t addr = 0; addr < (8 * 1024 * 1024); addr += 2) {
        uint16_t result = psram_read16(&psram_spi, addr);
        if ((uint16_t)(
                (((addr + 1) & 0xFF) << 8) |
                (addr & 0xFF)) != result
        ) {
            printf("PSRAM failure at address %x (%x != %x) ", addr, (
                (((addr + 1) & 0xFF) << 8) |
                (addr & 0xFF)), result
            );
            return 1;
        }
    }
    psram_elapsed = (time_us_32() - psram_begin);
    psram_speed = 1000000.0 * 8 * 1024 * 1024 / psram_elapsed;
    printf("16 bit: PSRAM read 8MB in %d us, %d B/s\n", psram_elapsed, (uint32_t)psram_speed);

    // **************** 32 bits testing ****************
    psram_begin = time_us_32();
    for (uint32_t addr = 0; addr < (8 * 1024 * 1024); addr += 4) {
        psram_write32(
            &psram_spi, addr,
            (uint32_t)(
                (((addr + 3) & 0xFF) << 24) |
                (((addr + 2) & 0xFF) << 16) |
                (((addr + 1) & 0xFF) << 8)  |
                (addr & 0XFF))
        );
    }
    psram_elapsed = time_us_32() - psram_begin;
    psram_speed = 1000000.0 * 8 * 1024.0 * 1024 / psram_elapsed;
    printf("32 bit: PSRAM write 8MB in %d us, %d B/s\n", psram_elapsed, (uint32_t)psram_speed);

    psram_begin = time_us_32();
    for (uint32_t addr = 0; addr < (8 * 1024 * 1024); addr += 4) {
        uint32_t result = psram_read32(&psram_spi, addr);
        if ((uint32_t)(
            (((addr + 3) & 0xFF) << 24) |
            (((addr + 2) & 0xFF) << 16) |
            (((addr + 1) & 0xFF) << 8)  |
            (addr & 0XFF)) != result
        ) {
            printf("PSRAM failure at address %x (%x != %x) ", addr, (
                (((addr + 3) & 0xFF) << 24) |
                (((addr + 2) & 0xFF) << 16) |
                (((addr + 1) & 0xFF) << 8)  |
                (addr & 0XFF)), result
            );
            return 1;
        }
    }
    psram_elapsed = (time_us_32() - psram_begin);
    psram_speed = 1000000.0 * 8 * 1024 * 1024 / psram_elapsed;
    printf("32 bit: PSRAM read 8MB in %d us, %d B/s\n", psram_elapsed, (uint32_t)psram_speed);

    // **************** n bits testing ****************
    uint8_t write_data[256];
    for (size_t i = 0; i < 256; ++i) {
        write_data[i] = i;
    }
    psram_begin = time_us_32();
    for (uint32_t addr = 0; addr < (8 * 1024 * 1024); addr += 256) {
        for (uint32_t step = 0; step < 256; step += 16) {
            psram_write(&psram_spi, addr + step, write_data + step, 16);
        }
    }
    psram_elapsed = time_us_32() - psram_begin;
    psram_speed = 1000000.0 * 8 * 1024.0 * 1024 / psram_elapsed;
    printf("128 bit: PSRAM write 8MB in %d us, %d B/s\n", psram_elapsed, (uint32_t)psram_speed);

    psram_begin = time_us_32();
    uint8_t read_data[16];
    for (uint32_t addr = 0; addr < (8 * 1024 * 1024); addr += 256) {
        for (uint32_t step = 0; step < 256; step += 16) {
            psram_read(&psram_spi, addr + step, read_data, 16);
            if (memcmp(read_data, write_data + step, 16) != 0) {
                printf("PSRAM failure at address %x", addr);
                return 1;
            }
        }
    }
    psram_elapsed = time_us_32() - psram_begin;
    psram_speed = 1000000.0 * 8 * 1024.0 * 1024 / psram_elapsed;
    printf("128 bit: PSRAM read 8MB in %d us, %d B/s\n", psram_elapsed, (uint32_t)psram_speed);


}
