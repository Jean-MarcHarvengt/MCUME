cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

# We also need PICO EXTRAS
#include(pico_extras_import.cmake)

project(mcume C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

add_subdirectory(FatFs_SPI build)

include_directories(config)
include_directories(vga_t4)
include_directories(tft_t)


set(PICO81_SOURCES 
		pico81/Z80.c 
		pico81/AY8910.c
		pico81/zx81.c
		pico81/emuapi.cpp
		pico81/pico81.cpp
		pico81/AudioPlaySystem.cpp
	)

set(PICOSPECCY_SOURCES 
		picospeccy/Z80.c 
		picospeccy/AY8910.c
		picospeccy/spec.c
		picospeccy/zx_filetyp_z80.c
		picospeccy/emuapi.cpp
		picospeccy/picospeccy.cpp
		picospeccy/AudioPlaySystem.cpp
	)

set(PICO800_SOURCES 
		pico800/antic.c 
		pico800/atari800.c
		pico800/cpu.c
		pico800/crc32.c
		pico800/gtia.c
		pico800/pia.c
		pico800/pokey.c
		pico800/pokeysnd.c
		pico800/sio.c
		pico800/emuapi.cpp
		pico800/AudioPlaySystem.cpp
		pico800/pico800.cpp
	)

set(PICO5200_SOURCES 
		pico5200/antic.c 
		pico5200/atari5200.c
		pico5200/cpu.c
		pico5200/crc32.c
		pico5200/gtia.c
		pico5200/pokey.c
		pico5200/pokeysnd.c
		pico5200/emuapi.cpp
		pico5200/AudioPlaySystem.cpp
		pico5200/pico5200.cpp
	)

set(PICO64_SOURCES 
		pico64/c64.cpp 
		pico64/cia1.cpp
		pico64/cia2.cpp
		pico64/cpu.cpp
		pico64/patches.cpp
		pico64/pla.cpp
		pico64/roms.cpp
		pico64/sid.cpp
		pico64/util.cpp
		pico64/vic.cpp
		pico64/reSID.cpp
		pico64/emuapi.cpp
		pico64/AudioPlaySystem.cpp
		pico64/pico64.cpp
	)

set(PICOCOLEM_SOURCES 
		picocolem/Z80.c 
		picocolem/SN76489.c
		picocolem/Colem.c
		picocolem/emuapi.cpp
		picocolem/picocolem.cpp
		picocolem/AudioPlaySystem.cpp
	)

set(PICOO2EM_SOURCES 
		picoo2em/audio.c 
		picoo2em/cpu.c
		picoo2em/crc32.c
		picoo2em/cset.c
		picoo2em/Oddemu.c
		picoo2em/table.c
		picoo2em/vdc.c
		picoo2em/vmachine.c
		picoo2em/vpp_cset.c
		picoo2em/vpp.c
		picoo2em/emuapi.cpp
		picoo2em/picoo2em.cpp
		picoo2em/AudioPlaySystem.cpp
	)

set(PICOVCS_SOURCES 
		picovcs/At2600.c 
		picovcs/Collision.c
		picovcs/Cpu.c
		picovcs/Display.c
		picovcs/Exmacro.c
		picovcs/Keyboard.c
		picovcs/Memory.c
		picovcs/Options.c
		picovcs/Raster.c
		picovcs/Table.c
		picovcs/Tiasound.c
		picovcs/Vcsemu.c
		picovcs/Vmachine.c
		picovcs/emuapi.cpp
		picovcs/picovcs.cpp
		picovcs/AudioPlaySystem.cpp
	)

set(PICONOFRENDO_SOURCES 
		piconofrendo/bitmap.c 
		piconofrendo/config.c
		piconofrendo/event.c
		piconofrendo/log.c
		piconofrendo/map000.c
		piconofrendo/map001.c
		piconofrendo/map002.c
		piconofrendo/map003.c
		piconofrendo/map004.c
		piconofrendo/map005.c
		piconofrendo/map007.c
		piconofrendo/map008.c
		piconofrendo/map009.c
		piconofrendo/map011.c
		piconofrendo/map015.c
		piconofrendo/map016.c
		piconofrendo/map018.c
		piconofrendo/map019.c
		piconofrendo/map024.c
		piconofrendo/map032.c
		piconofrendo/map033.c
		piconofrendo/map034.c
		piconofrendo/map040.c
		piconofrendo/map041.c
		piconofrendo/map042.c
		piconofrendo/map046.c
		piconofrendo/map050.c
		piconofrendo/map064.c
		piconofrendo/map065.c
		piconofrendo/map066.c
		piconofrendo/map070.c
		piconofrendo/map073.c
		piconofrendo/map075.c
		piconofrendo/map078.c
		piconofrendo/map079.c
		piconofrendo/map085.c
		piconofrendo/map087.c
		piconofrendo/map093.c
		piconofrendo/map094.c
		piconofrendo/map099.c
		piconofrendo/map160.c
		piconofrendo/map229.c
		piconofrendo/map231.c
		piconofrendo/mapvrc.c
		piconofrendo/mmc5_snd.c
		piconofrendo/mmclist.c
		piconofrendo/nes_apu.c
		piconofrendo/nes_emu.c
		piconofrendo/nes_mmc.c
		piconofrendo/nes_pal.c
		piconofrendo/nes_ppu.c
		piconofrendo/nes_rom_light.c
		piconofrendo/nes.c
		piconofrendo/nes6502.c
		piconofrendo/nesinput.c
		piconofrendo/nofrendo.c
		piconofrendo/vid_drv.c
		piconofrendo/vrcvisnd.c
		piconofrendo/emuapi.cpp
		piconofrendo/piconofrendo.cpp
		piconofrendo/AudioPlaySystem.cpp
	)

set(PICOSND_SOURCES 
#		picosnd/LibFC14/Dump.cpp
#		picosnd/LibFC14/FC_Data.cpp
#		picosnd/LibFC14/FC.cpp
#		picosnd/LibFC14/fc14audiodecoder.cpp
#		picosnd/LibFC14/LamePaulaMixer.cpp
#		picosnd/LibFC14/LamePaulaVoice.cpp
#		picosnd/LibFC14/Paula.cpp
#		picosnd/StSnd/digidrum.cpp
#		picosnd/StSnd/LzhLib.cpp
#		picosnd/StSnd/Ym2149Ex.cpp
#		picosnd/StSnd/Ymload.cpp
#		picosnd/StSnd/YmMusic.cpp
#		picosnd/StSnd/YmUserInterface.cpp
		picosnd/sndplay.cpp
		picosnd/sid.cpp
		picosnd/reSID.cpp
		picosnd/emuapi.cpp
		picosnd/AudioPlaySystem.cpp
		picosnd/picosnd.cpp
	)

set(VGA_T4_SOURCES 
		vga_t4/VGA_t4.cpp
		vga_t4/scanvideo.c
	)

set(TFT_T_SOURCES 
		tft_t/tft_t_dma.cpp
	)

set(GFXENGINE_SOURCES 
		gfxengine/gfxengine.cpp
	)

set(TESTIO_SOURCES 
		testio/testio.cpp
		testio/emuapi.cpp
	)

set(TESTVGA_SOURCES 
		testvga/testvga.cpp
	)

set(TESTTFT_SOURCES 
		testtft/testtft.cpp
		testtft/emuapi.cpp
	)

add_executable(mcume
#		${GFXENGINE_SOURCES}
#		${PICO81_SOURCES}
#		${PICOSPECCY_SOURCES}
#		${PICO800_SOURCES}
#		${PICO5200_SOURCES}
		${PICO64_SOURCES}
#		${PICOCOLEM_SOURCES}
#		${PICOO2EM_SOURCES}
#		${PICOVCS_SOURCES}
#		${PICONOFRENDO_SOURCES}
#		${PICOSND_SOURCES}
#		${TESTIO_SOURCES}
#		${TESTVGA_SOURCES}
#		${TESTTFT_SOURCES}
		${VGA_T4_SOURCES}
		${TFT_T_SOURCES}
	)

pico_generate_pio_header(mcume ${CMAKE_CURRENT_LIST_DIR}/vga_t4/timing.pio)
pico_generate_pio_header(mcume ${CMAKE_CURRENT_LIST_DIR}/vga_t4/scanvideo.pio)


target_link_libraries(mcume pico_multicore
	    pico_stdlib
	    FatFs_SPI
#	    pico_scanvideo_dpi
	    hardware_adc
	    hardware_pio
        hardware_pwm	    
	)


#target_compile_options(mcume PUBLIC -O3)
#target_compile_options(mcume PUBLIC -Wall -Wextra -Wno-unused-function -Wno-unused-parameter)
IF (NOT DEFINED N_SD_CARDS)
    SET(N_SD_CARDS 1)
ENDIF()
target_compile_definitions(mcume PUBLIC N_SD_CARDS=${N_SD_CARDS})


pico_enable_stdio_uart(mcume 0)
pico_enable_stdio_usb(mcume 0)

pico_add_extra_outputs(mcume)
