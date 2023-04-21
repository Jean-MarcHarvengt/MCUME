cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

# We also need PICO EXTRAS
#include(pico_extras_import.cmake)

#set(TARGET testkeymax)
#set(TARGET pico20)
set(TARGET pico64)
#set(TARGET pico81)
#set(TARGET picospeccy)
#set(TARGET pico800)
#set(TARGET pico5200)
#set(TARGET picocolem)
#set(TARGET picoo2em)
#set(TARGET picovcs)

project(${TARGET} C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

add_subdirectory(FatFs_SPI build)

include_directories(${TARGET})
include_directories(config)
include_directories(display)
#include_directories(vga_t4)
#include_directories(picovga_t4)
#include_directories(tft_t)
include_directories(psram)

if( ${TARGET} MATCHES "pico20" )
set(PICO20_SOURCES 
		pico20/IC.cpp
		pico20/mos6502.cpp
		pico20/MOS6522.cpp
		pico20/MOS6561.cpp
		pico20/v20.cpp
		pico20/pico20.cpp
	)
#add_compile_definitions(OVERRULE_WIDTH=320 OVERRULE_HEIGHT=192)	
endif()

if( ${TARGET} MATCHES "pico64" )
set(PICO64_SOURCES 
		pico64/c64.cpp 
		pico64/cia1.cpp
		pico64/cia2.cpp
		pico64/cpu.cpp
		pico64/patches.cpp
		pico64/pla.cpp
		pico64/roms.cpp
		pico64/sid.cpp
		pico64/timerutil.cpp
		pico64/vic.cpp
		pico64/reSID.cpp
		pico64/pico64.cpp
	)
endif()

if( ${TARGET} MATCHES "pico81" )
set(PICO81_SOURCES 
		pico81/Z80.c 
		pico81/AY8910.c
		pico81/zx81.c
		pico81/pico81.cpp
	)
# ZX81,ZX Spectrum,Colem,Vic20
add_compile_definitions(OVERRULE_WIDTH=320 OVERRULE_HEIGHT=192)	
endif()

if( ${TARGET} MATCHES "picospeccy" )
set(PICOSPECCY_SOURCES 
		picospeccy/Z80.c 
		picospeccy/AY8910.c
		picospeccy/spec.c
		picospeccy/zx_filetyp_z80.c
		picospeccy/picospeccy.cpp
	)
add_compile_definitions(OVERRULE_WIDTH=320 OVERRULE_HEIGHT=192)	
endif()

if( ${TARGET} MATCHES "pico800" )
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
		pico800/pico800.cpp
	)
endif()

if( ${TARGET} MATCHES "pico5200" )
set(PICO5200_SOURCES 
		pico5200/antic.c 
		pico5200/atari5200.c
		pico5200/cpu.c
		pico5200/crc32.c
		pico5200/gtia.c
		pico5200/pokey.c
		pico5200/pokeysnd.c
		pico5200/pico5200.cpp
	)
endif()

if( ${TARGET} MATCHES "picocolem" )
set(PICOCOLEM_SOURCES 
		picocolem/Z80.c 
		picocolem/SN76489.c
		picocolem/Colem.c
		picocolem/picocolem.cpp
	)
add_compile_definitions(OVERRULE_WIDTH=320 OVERRULE_HEIGHT=192)	
endif()

if( ${TARGET} MATCHES "picoo2em" )
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
		picoo2em/picoo2em.cpp
	)
add_compile_definitions(OVERRULE_WIDTH=320 OVERRULE_HEIGHT=192)	
endif()

if( ${TARGET} MATCHES "picovcs" )
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
		picovcs/picovcs.cpp
	)
endif()

if( ${TARGET} MATCHES "piconofrendo" )
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
		piconofrendo/piconofrendo.cpp
	)
endif()

if( ${TARGET} MATCHES "pico8086" )
set(PICO8086_SOURCES 
		pico8086/cpu.cpp 
		pico8086/disk.cpp
		pico8086/network.cpp
		pico8086/ports.cpp
		pico8086/i8253.cpp
		pico8086/i8259.cpp
		pico8086/pc.cpp
		pico8086/pico8086.cpp
	)
endif()

if( ${TARGET} MATCHES "testio" )
set(TESTIO_SOURCES 
		testio/testio.cpp
		testio/emuapi.cpp
	)
endif()

if( ${TARGET} MATCHES "testvga" )
set(TESTVGA_SOURCES 
		testvga/testvga.cpp
	)
endif()

if( ${TARGET} MATCHES "testkeymax" )
set(TESTKEYMAX_SOURCES 
		testkeymax/testkeymax.cpp
	)
endif()

set(DISPLAY_SOURCES 
		display/pico_dsp.cpp
		display/vga.cpp
		display/vga_vmode.cpp
		display/emuapi.cpp
		display/AudioPlaySystem.cpp		
	)

#if( ${TARGET} MATCHES "gfxengine" )
#set(GFXENGINE_SOURCES 
#		gfxengine/gfxengine.cpp
#	)
#endif()

#set(PICOSND_SOURCES 
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
#		picosnd/sndplay.cpp
#		picosnd/sid.cpp
#		picosnd/reSID.cpp
#		picosnd/emuapi.cpp
#		picosnd/AudioPlaySystem.cpp
#		picosnd/picosnd.cpp
#	)

#set(PICOVALIDATION_SOURCES 
#		picovalidation/c64.cpp 
#		picovalidation/cia1.cpp
#		picovalidation/cia2.cpp
#		picovalidation/cpu.cpp
#		picovalidation/patches.cpp
#		picovalidation/pla.cpp
#		picovalidation/roms.cpp
#		picovalidation/sid.cpp
#		picovalidation/util.cpp
#		picovalidation/vic.cpp
#		picovalidation/reSID.cpp
#		picovalidation/emuapi.cpp
#		picovalidation/AudioPlaySystem.cpp
#		picovalidation/pico64.cpp
#	)

#set(VGA_T4_SOURCES 
#		vga_t4/VGA_t4.cpp
#		vga_t4/scanvideo.c
#	)

#set(PICOVGA_T4_SOURCES 
#		picovga_t4/VGA_t4.cpp
#		picovga_t4/vga.cpp
#		picovga_t4/vga_vmode.cpp
#	)

#set(PSRAM_SOURCES 
#		psram/psram_t.cpp
#	)

#set(TFT_T_SOURCES 
#		tft_t/tft_t_dma.cpp
#	)

#set(TESTPSRAM_SOURCES 
#		testpsram/testpsram.cpp
#		testpsram/emuapi.cpp
#	)


add_executable(${TARGET}
		${PICO20_SOURCES}
		${PICO64_SOURCES}
		${PICO81_SOURCES}
		${PICO800_SOURCES}
		${PICO5200_SOURCES}
		${PICO8086_SOURCES}
		${PICOCOLEM_SOURCES}
		${PICOO2EM_SOURCES}
		${PICOVCS_SOURCES}
		${PICOSPECCY_SOURCES}
		${PICONOFRENDO_SOURCES}
		${TESTIO_SOURCES}
		${TESTVGA_SOURCES}
		${TESTKEYMAX_SOURCES}
#		${GFXENGINE_SOURCES}
#		${PICOSND_SOURCES}
#		${TESTPSRAM_SOURCES}
#		${PICOVALIDATION_SOURCES}
#		${PSRAM_SOURCES}
#		${VGA_T4_SOURCES}
#		${PICOVGA_T4_SOURCES}
#		${TFT_T_SOURCES}
		${DISPLAY_SOURCES}
	)

#pico_generate_pio_header(${TARGET} ${CMAKE_CURRENT_LIST_DIR}/vga_t4/timing.pio)
#pico_generate_pio_header(${TARGET} ${CMAKE_CURRENT_LIST_DIR}/vga_t4/scanvideo.pio)
pico_generate_pio_header(${TARGET} ${CMAKE_CURRENT_LIST_DIR}/display/picovga.pio)


target_link_libraries(${TARGET} pico_multicore
	    pico_stdlib
	    FatFs_SPI
#	    pico_scanvideo_dpi
	    hardware_adc
	    hardware_pio
        hardware_pwm	    
	)


#target_compile_options(${TARGET} PUBLIC -O3)
#target_compile_options(${TARGET} PUBLIC -Wall -Wextra -Wno-unused-function -Wno-unused-parameter)
IF (NOT DEFINED N_SD_CARDS)
    SET(N_SD_CARDS 1)
ENDIF()
target_compile_definitions(${TARGET} PUBLIC N_SD_CARDS=${N_SD_CARDS})


#pico_enable_stdio_uart(${TARGET} 0)
#pico_enable_stdio_usb(${TARGET} 0)

pico_add_extra_outputs(${TARGET})
