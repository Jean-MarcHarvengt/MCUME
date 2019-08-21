// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "esp_log.h"
#include "rom/gpio.h"
#include "rom/spi_flash.h"
#include "bootloader_config.h"
#include "bootloader_init.h"
#include "bootloader_utility.h"
#include "bootloader_common.h"
#include "sdkconfig.h"
#include "esp_image_format.h"

#include <rom/rtc.h>

static const char* TAG = "boot";

#define RTC_SLOW_MEM ((uint32_t*) 0x50000000)       /*!< RTC slow memory, 8k size */

#define ULP_DATA_OFFSET     36

_Static_assert(ULP_DATA_OFFSET < CONFIG_ULP_COPROC_RESERVE_MEM/4 - 6,
        "ULP_DATA_OFFSET is set too high, or CONFIG_ULP_COPROC_RESERVE_MEM is not sufficient");

static inline uint16_t ulp_data_read(size_t offset)
{
    return RTC_SLOW_MEM[ULP_DATA_OFFSET + offset] & 0xffff;
}
static inline void ulp_data_write(size_t offset, uint16_t value)
{
    RTC_SLOW_MEM[ULP_DATA_OFFSET + offset] = value;
}

static int select_partition_number (bootloader_state_t *bs);
static int selected_boot_partition(const bootloader_state_t *bs);
/*
 * We arrive here after the ROM bootloader finished loading this second stage bootloader from flash.
 * The hardware is mostly uninitialized, flash cache is down and the app CPU is in reset.
 * We do have a stack, so we can do the initialization in C.
 */
void __attribute__((noreturn)) call_start_cpu0()
{
    // 1. Hardware initialization
    if (bootloader_init() != ESP_OK) {
        bootloader_reset();
    }

    ESP_LOGI(TAG, "JMH bootloader");

    // 2. Select the number of boot partition
    bootloader_state_t bs = { 0 };
    int boot_index = select_partition_number(&bs);
    if (boot_index == INVALID_INDEX) {
        bootloader_reset();
    }

    // 3. Load the app image for booting
    bootloader_utility_load_boot_image(&bs, boot_index);
}

// Select the number of boot partition
static int select_partition_number (bootloader_state_t *bs)
{
    // 1. Load partition table
    if (!bootloader_utility_load_partition_table(bs)) {
        ESP_LOGE(TAG, "load partition table error!");
        return INVALID_INDEX;
    }

    // 2. Select the number of boot partition
    return selected_boot_partition(bs);
}

/*
 * Selects a boot partition.
 * The conditions for switching to another firmware are checked.
 */
static int selected_boot_partition(const bootloader_state_t *bs)
{
    int boot_index = bootloader_utility_get_selected_boot_partition(bs);
    if (boot_index == INVALID_INDEX) {
        return boot_index; // Unrecoverable failure (not due to corrupt ota data or bad partition contents)
    } else {
        if (rtc_get_reset_reason(0) == 12) {
            boot_index = ulp_data_read(0);
        }    
        ESP_LOGI(TAG, "JMH reset reason is %d", rtc_get_reset_reason(0));
        ESP_LOGI(TAG, "JMH ULP mem is %d", ulp_data_read(0));
        ESP_LOGI(TAG, "JMH partition index is %d", boot_index);
    }
    return boot_index;
}
