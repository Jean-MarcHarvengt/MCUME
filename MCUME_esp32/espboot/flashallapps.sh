#!/bin/bash
. ${IDF_PATH}/add_path.sh
esptool.py --chip esp32 --port "/dev/cu.SLAB_USBtoUART" --baud $((230400*4)) write_flash -fs 4MB 0x080000 ../esp81/build/esp81.bin 0x0F0000 ../espspeccy/build/espspeccy.bin 0x160000 ../esp800/build/esp800.bin 0x1D0000 ../esp64/build/esp64.bin 0x240000 ../espvcs/build/espvcs.bin 0x2B0000 ../espo2em/build/espo2em.bin 0x320000 ../espcolem/build/espcolem.bin 0x390000 ../esp5200/build/esp5200.bin
