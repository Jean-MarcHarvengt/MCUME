cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

# We also need PICO EXTRAS
#include(pico_extras_import.cmake)

set(TARGET testkeymax)
#set(TARGET pico20)
#set(TARGET pico64)
#set(TARGET pico81)
#set(TARGET pico800)
#set(TARGET picospeccy)
#set(TARGET picomsx)
#set(TARGET pico8086)
#set(TARGET picocastaway)
#set(TARGET picocolem)
#set(TARGET picoo2em)
#set(TARGET picovcs)
#set(TARGET picogb)
#set(TARGET picopce)
#set(TARGET picosms)
#set(TARGET picogen)

#set(TARGET pico5200)

project(${TARGET} C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

include_directories(${TARGET})
include_directories(config)
include_directories(display)
include_directories(fatfs)
include_directories(fatfs/source)
include_directories(sd_driver)
include_directories(psram)
include_directories(flash)
include_directories(usb_kbd)
include_directories(.)

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


if( ${TARGET} MATCHES "picosms" )
set(PICOSMS_SOURCES 
		picosms/emu.cpp 
		picosms/fmopl.c
		picosms/memory.c
		picosms/render.c
		picosms/sms.c
		picosms/sn76496.c
		picosms/system.c
		picosms/vdp.c
		picosms/ym2413.c
		picosms/z80.c
		picosms/picosms.cpp
	)
endif()

if( ${TARGET} MATCHES "pico8086" )
set(PICO8086_SOURCES 
		pico8086/emu.cpp
		pico8086/cpu.cpp 
		pico8086/disk.cpp
		pico8086/network.cpp
		pico8086/ports.cpp
		pico8086/i8253.cpp
		pico8086/i8259.cpp
		pico8086/pico8086.cpp
	)
endif()

if( ${TARGET} MATCHES "picopce" )
set(PICOPCE_SOURCES 
		picopce/emu.cpp 
		picopce/pce-go/gfx.c
		picopce/pce-go/h6280.c
		picopce/pce-go/pce.c
		picopce/pce-go/psg.c
		picopce/pce-go/pce-go.c
		picopce/picopce.cpp
	)
endif()

if( ${TARGET} MATCHES "picomsx" )
set(PICOMSX_SOURCES 
		picomsx/fmsx.c 
		picomsx/AY8910.c
		picomsx/Boot.c
		picomsx/Disk.c
		picomsx/I8251.c
		picomsx/I8255.c
		picomsx/Patch.c
		picomsx/SCC.c
		picomsx/Sound.c
		picomsx/V9938.c
		picomsx/YM2413.c
		picomsx/Z80.c
		picomsx/picomsx.cpp
	)
endif()

if( ${TARGET} MATCHES "picogen" )

set(PICOGEN_SOURCES 
		picogen/emu.cpp 
		picogen/gwenesis/bus/gwenesis_bus.c
		picogen/gwenesis/cpus/M68K/m68kcpu.c
		picogen/gwenesis/cpus/Z80/Z80.c
		picogen/gwenesis/cpus/Z80/Debug.c
		picogen/gwenesis/cpus/Z80/ConDebug.c
		picogen/gwenesis/io/gwenesis_io.c
		picogen/gwenesis/sound/gwenesis_sn76489.c
		picogen/gwenesis/sound/ym2612.c
		picogen/gwenesis/sound/z80inst.c
		picogen/gwenesis/vdp/gwenesis_vdp_gfx.c
		picogen/gwenesis/vdp/gwenesis_vdp_mem.c
		picogen/picogen.cpp		
	)

file(GLOB SPECIAL_SRC_FILES
		"picogen/gwenesis/bus/gwenesis_bus.c"
		"picogen/gwenesis/cpus/M68K/m68kcpu.c"
		"picogen/gwenesis/cpus/Z80/Z80.c"
		"picogen/gwenesis/cpus/Z80/Debug.c"
		"picogen/gwenesis/cpus/Z80/ConDebug.c"
		"picogen/gwenesis/io/gwenesis_io.c"
		"picogen/gwenesis/sound/gwenesis_sn76489.c"
		"picogen/gwenesis/sound/ym2612.c"
		"picogen/gwenesis/sound/z80inst.c"
		"picogen/gwenesis/vdp/gwenesis_vdp_gfx.c"
		"picogen/gwenesis/vdp/gwenesis_vdp_mem.c" )
set_source_files_properties(SOURCE ${SPECIAL_SRC_FILES} PROPERTIES COMPILE_FLAGS "-funroll-loops  -ffast-math -feliminate-unused-debug-types -ffunction-sections -fdata-sections -O2")

endif()

if( ${TARGET} MATCHES "picogb" )
set(PICOGB_SOURCES 
		picogb/emu.cpp 
		picogb/minigb_apu/minigb_apu.c
		picogb/picogb.cpp
	)
endif()

if( ${TARGET} MATCHES "picocastaway" )
set(PICOCASTAWAY_SOURCES 
		picocastaway/emu.cpp 
		picocastaway/blitter.cpp
		picocastaway/famec.cpp
		picocastaway/fdc.cpp
		picocastaway/ikbd.cpp
		picocastaway/m68k_intrf.cpp
		picocastaway/mem.cpp
		picocastaway/st.cpp
		picocastaway/sound.cpp
		picocastaway/picocastaway.cpp
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
		display/hdmi_framebuffer.cpp		
		display/emuapi.cpp
		display/AudioPlaySystem.cpp		
	)

set(USB_SOURCES 
		usb_kbd/hid_app.c
	)

set(FATSD_SOURCES 
		sd_driver/tf_card.c
		fatfs/source/ff.c
		fatfs/source/ffunicode.c		
		fatfs/source/ffsystem.c		
	)


set(PSRAM_SOURCES 
		psram/psram_spi.c
		psram/psram_t.cpp
	)

set(FLASH_SOURCES 
		flash/flash_t.c
	)

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
		${PICOSMS_SOURCES}
		${PICOGEN_SOURCES}
		${PICOPCE_SOURCES}
		${PICOGB_SOURCES}
		${PICOMSX_SOURCES}
		${PICOCASTAWAY_SOURCES}
		${TESTIO_SOURCES}
		${TESTVGA_SOURCES}
		${TESTKEYMAX_SOURCES}
		${PSRAM_SOURCES}
		${FLASH_SOURCES}
		${DISPLAY_SOURCES}
		${FATSD_SOURCES}
		${USB_SOURCES}
	)

pico_generate_pio_header(${TARGET} ${CMAKE_CURRENT_LIST_DIR}/psram/psram_spi.pio)

target_link_libraries(${TARGET} pico_multicore
	    pico_stdlib
	    hardware_dma
	    hardware_spi
	    hardware_adc
	    hardware_pio
        hardware_pwm	    
        tinyusb_host 
        tinyusb_board
		tinyusb_pico_pio_usb
	)

target_compile_definitions(${TARGET} PRIVATE
        PIO_USB_DP_PIN_DEFAULT=2
    )

target_compile_definitions(${TARGET} PRIVATE PICO_CLOCK_AJDUST_PERI_CLOCK_WITH_SYS_CLOCK=1)


#target_compile_definitions(${TARGET} PRIVATE CFG_TUSB_DEBUG=2)
#target_compile_options(${TARGET} PUBLIC -O3)
#target_compile_options(${TARGET} PUBLIC -Wall -Wextra -Wno-unused-function -Wno-unused-parameter)
IF (NOT DEFINED N_SD_CARDS)
    SET(N_SD_CARDS 1)
ENDIF()
target_compile_definitions(${TARGET} PUBLIC N_SD_CARDS=${N_SD_CARDS})


pico_enable_stdio_uart(${TARGET} FALSE)
#pico_enable_stdio_usb(${TARGET} TRUE)

pico_add_extra_outputs(${TARGET})
