# M.CU.M.E = Multi CompUter Machine Emulator
<p align="center">
  <img width="320" height="140" src="/images/logo.png">
</p>

# News
Mars 2025: Finally some updates! Add pico2 support for hyperpetpico HDMI and PICO2ZX (Bobricus)
- HDMI now supported next to TFT (pico2 only)
- Add USB HOST (pico2 only)
- Port of new emulators to pico2: pcengine, sega master system, sega genesis (exp), gameboy, msx, pc8086, Atari 260ST(exp)
- (thanks to the original authors of the code, I only did the port)
- Instructions when using PICO2ZX:
  - use binaries in /bin ("pico2zx/tft_hdmi" if you have a TFT+HDMI or "pico2zx/hdmi_only" if only have HDMI)
  - USR1 = KEMPSTON(>>>>) key, USR2 = MENU(QSAVE) key, USR3 = CURSOR(<<<<) key, DELETE and FIRE have same effect
  - arrow up + reset => use HDMI default (TFT+HDMI binaries)
  - arrow left/right + reset => use TFT default (TFT+HDMI binaries)
  - FIRE: launch game to TFT (TFT+HDMI binaries)
  - USR1: launch game to HDMI (TFT+HDMI binaries)
  - USR3: select + launch rom/game in AUTORUN
  - arrow down + reset => delete autorun mode
<br>

Mai 2023: add multi display support to T-COMPUTER and PICOZX platforms
- You can now configure at boot the display to use for each emulator (Teensy41 or PICO)
- No need for different binaries depending on the display (VGA,ILI and ST displays are all suported)
- Config is stored on the SDCARD
- Press Up while booting to force VGA output at startup (will erase display config => default VGA)
- Press Left while booting to force ST7789 output at startup (will write config)
- Press Right while booting to force ILI9341 output at startup (will write config)
- When configured for non VGA display, you can always push to VGA by pressing ALT iso TAB key to start (on T-COMPUTER) 
- Add FMSX and SNES emulators to T-COMPUTER. 
- Port of all PICO emulators to the new PICOZX platform of BOBRICIUS
<br>

February 2022: introducing T(TEE)-COMPUTER platform based on Teensy41 MCU:<br>
- T-COMPUTER has VGA and TFT video out, Audio out, USB input, embedded 42 keys keyboard and a DB9 joystick input!
- now supported on T-COMPUTER: Apple2, Vic20, C64, Atari800, ZX81, ZXSpectrum, Atari ST, Amiga, Colecovision, Odyssey, NES, SMS, Genesis and Doom.
- new emus for Teensy41: Vic20 and Apple2 based on aiie!
- add AUTORUN feature to Teensy and PICO platforms
- add Floppy write support to Castaway ST emulator
- improved USB keyboard support for existing Teensy41 emus
<br>

December 2021:<br>
- Add vic20 emulation to the PICO
<br>

October 2021: yet more PICO platforms improvements<br>
- MCUME_REV2 has now dual screen support (TFT+VGA binaries where TFT can be used for keyboard control, or VGA only binaries wo keyboard control)
- Fix keyboard scan for PICOMPUTERMAX (C64,ZX81,ZX Spectrum and Atari800 have now usable keyboard for basic program typing)
<br>

September 2021: More PICO platforms improvements<br>
- MCUME_REV2 has now digital inputs for UP/DOWN/LEFT/RIGHT so external C64 joystick can be connected (VGA & TFT binaries)
- PICOMPUTERMAX now supporting ALT and FN(long press on ALT) so more C64 keys can be mapped (FN for Function keys for e.g.)
<br>


<details>
  <summary>Click for more news</summary>
  
<br>
August 2021: Add support for PICOMPUTERMAX on PICO<br>
- TFT 7789 320x240
<br>

July 2021: Add support for TFT dispays on PICO<br>
- MCUME_REV2 supports VGA and TFT (ST or ILI) on same layout
- Add specific support for PICOMPUTER (TFT and full keyboard microswitch)<br>
https://www.youtube.com/watch?v=4tL9K5tXZtY
<br>

May 13th 2021: C64 emulator improvement for PICO<br>
- Overclocked to 250MHz for better speed
- Add on screen keyboard for text input
<br>

May 2021: few improvements for raspberry pi PICO<br>
- VGA driver stripped down for 8bits RRRGGGBB (red lines bug fixed)...
- Improved PWM audio...
- Add sound to C64 emulator but still running far too slow
- Add SID player application (picosnd)
<br>

April 2021: Add initial support for raspberry PICO<br>
- adapted pico scanline library for  VGA 8 bits mode (RRRGGGBB), sorry adapted code is embedded...
- SD support from https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico (fixed IRQ to 1)
- rudimentary PWM IRQ based audio (https://gregchadwick.co.uk/blog/playing-with-the-pico-pt3/)

Far from perfect, lot of bugs, and ... unfortunately C64 emulator has no sound for performance reason!<br>
All what you need is in MCUME_pico. Precompiled binaries in MCUME_pico/bin.<br>
To recompile, look at MCUME_pico/readme.txt<br>

https://www.youtube.com/watch?v=MUpwlZXheb8<br>
https://www.youtube.com/watch?v=_WL_KGkxZ88
<br><br>

November 2020 UPDATE: Atari ST emulation now supporting MIDI output!<br>
https://www.youtube.com/watch?v=H-TlCDBwdak<br>
https://www.youtube.com/watch?v=fzoTYUpEOKs


November 2020: move to latest VGA_t4 library for stable HIRES support<br>
- USB mouse added to Amiga/Atari ST emulation
- Amiga sound improved (freq drifting less audible)
- Atari ST now support 640x200 and 640x400 (B/W) modes with double buffering!!!
- Amiga now support 640x240 mode for cleaner workbench!!!<br>
https://www.youtube.com/watch?v=vex335vHc68<br>
https://www.youtube.com/watch?v=LKdcduVwwGQ


Mid October 2020: Most emulators are now supporing VGA with sound on the Teensy4.1<br>
Experimental I2S sound driver has been added to the VGA_t4 library with minimal DMA disturbances.<br><br>
If you are experimenting issues (e.g. image flashing out, crash after few minutes):
- try compiling for smallest code iso faster
<br>
e.g.:<br>
- ST emu: compile for "smallest code"<br>
- Amiga emu: compile for "smallest code"<br>
- Doom: compile for "faster"<br>

Atari ST emu has now better sound support!<br>
<br>

October 2020: DOOM ported to the Teensy4.1 (for VGA and LCD displays)<br>
No sound yet but with USB keyboard support (see VGA prototype PCB below).<br>
https://www.youtube.com/watch?v=adYwnctiKsg
<br>
<p align="center">
<img src="/images/T41VGA.jpg" width="640" />  
<br>
  
June 2020: Update of UAE (Amiga emulator) on Teensy4.1<br>
Now with HDD support, better sound and 68020 emulation.<br>
All for better compatibility...<br>
https://youtu.be/Ho8qZSCKdLg<br>
https://youtu.be/vuMYwylYqYs

May 2020: Amiga emulation on Teensy4.1 (with PSRAM) !!!<br>
Here is a video of the emulator running on my prototype board.<br>
https://youtu.be/LE4IWPG5z6g

March 2020: MSX1/2 emulation on Teensy4.0 (no SDRAM required)<br>
Mini-console prototype with 40 keys keyboard, keypad and 320x240 ST7789 display.
The prototype supports of course all systems below where keyboard is supported!<br> 
https://youtu.be/YZGbGKmeb6I<br>
<br>
<p align="center">
<img src="/images/msx.png" width="640" />  
<br>


Nov 2019: Gameboy, Sega Master System/ GameGear and Megadrive/Genesis emulator running with extra 8MB SPI RAM (PSRAM) <br>
https://youtu.be/j2sKw7KYpEo <br>

Oct 2019: PC-Engine 'tiny' emulator running on Teensy 4.0 using an extra 8MB SPI RAM (PSRAM)  <br>
https://youtu.be/Ot9RgDMqdF4<br>
GFX bug due to objs cache limitation is now fixed!
<br>
<p align="center">
<img src="/images/t4piggy.png" width="240" />  
<img src="/images/pcbT4.png" width="480" />  
<br>
T4 with PSRAM piggy back and latest T4 pcb!
</p>

<br>
<br>

Sep 2019: Teensy 4.0 now running Atari 520ST emulator (68k core based)!!! <br>
https://youtu.be/UCOaOGmYYso<br>
https://youtu.be/6rr2hMqprO0<br>
<br>

</details>



<p align="center">
<img src="/images/teensy4SD.png" width="200" />  
</p>

<p align="center">
<img src="/images/stdesktop.png" width="200" />  
<img src="/images/strobocop.png" width="200" />  
</p>

<p align="center">
<img src="/images/teensybread.jpg" width="200" />  
<img src="/images/teensy362.jpg" width="200" />  
</p>

<p align="center">
<img src="/images/esp321.jpg" width="200" />  
<img src="/images/esp322.jpg" width="200" />  
</p>
A collection of 80th game Console and Computer emulators ported to Teensy, raspberry PICO and ESP32 MCUs.<br>
Have a look at the video:
https://youtu.be/jlqoWH2X6gk<br><br>
This project is a spin-off of the TeensyCEC project https://github.com/Jean-MarcHarvengt/TeensyCEC<br>
Now ported to much more MCUs! <br>

# Featuring

Computer systems supported and status on various MCU platforms<br>

| System | Teensy3.6 | Teensy 4.0 | Teensy4.0 +PSRAM | Teensy4.1 +PSRAM | ESP32 | ESP32-Wrover | Pico | Pico2 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Zx81        | X | X | X | X | X | X | X | X |
| Zx spectrum | X | X | X | X | X | X | X | X |
| Atari 800   | X | X | X | X | X | X | X | X |
| C64         | X | X | X | X | X | X | X | X |
| VIC20       |   |   |   | X |   |   | X | X |
| Apple2      |   |   |   | X |   |   |   | - |
| Atari 520ST | - | full speed! | X | X (640x400!) | - | slow | - | - |
| Atari 260ST | - | - | - | - | - | - | - | X |
| 8086 XT PC  | - | full speed! | X | X | - | slow | - | X |
| MSX1/2      | - | full speed! | X | X | - | - | - | X |
| Amiga       | - | - | exp only! | X (640x240!) | - | - | - | - |
| Doom        | - | - | - | x | - | - | - | - |

Please compile for smallest code on the Teensy4.0 else you will run out of memory<br><br>
For Teensy 4.1 with PSRAM try compiling for smallest code (unless it complains at compilation).<br> 
Doom for example only works if compiled for smallest code!

Game console systems supported and status on various MCU platforms<br>

| System | Teensy3.6 | Teensy 4.0 | Teensy4 +PSRAM | Teensy4.1 +PSRAM | ESP32 | ESP32-Wrover | Pico | Pico2 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Atari 2600                   | X | X | X | X | X | X | X | X |
| Odyssey/Videopac             | X | X | X | X | X | X | X | X |
| Colecovision                 | X | X | X | X | X | X | X | - |
| Atari 5200                   | X | X | X | X | X | X | X | X |
| Nintendo NES                 | X | 384KB roms! | X | X | X | X | - | - |
| Vectrex                      | - | full speed! | X | X | - | slow | - | - |
| PC Engine                    | - | - | X | X | - | - | - | X |
| Gameboy/GBColor              | - | 490KB roms| X | X | - | - | - | X |
| Sega Master System/Game Gear | - | - | X | X | - | - | - | X |
| Sega Genesis/Megadrive       | - | - | X | X | - | - | - | X |

# Minimal requirements:
- Teensy3.6/Teensy4.0, ESP32 Node32S/Wrover SDK chip or Raspberry PICO/PICO2
- ILI9341 (Teensy/ESP32) or ST7789 (Teensy only) SPI display
- SD card (Teensy uses built-in uSD, ESP32 uses external one connected over SPI, e.g. ILI9341 integrated one)
- Analog joypad (Arduino or PSP like)
- 3 buttons (FIRE, USER1 and USER2)

# Optional requirements:
- I2C custom keyboard or integrated one (for computers emulators)
- Sound (DAC for ESP and Teensy3.6, MQS for Teensy4.0, I2S for Teensy4.1, PWM for PICO)
- VGA output (Teensy 3.6, Teensy 4.1 and Raspberry PICO only)
- 2 extra buttons (USER3 and USER4) mostly for NES emu

# Wiring
- see pinout.txt file in the respective project (ESP32 or Teensy)
- Some KICAD image also available for ESP32

# I2C keyboard (for computer emus on Teensy 3.6 only)
- see i2ckeyboard sub-directory
- the I2C keyboard is using a separate atmega328p MCU handling the keys matrix
- with 10x4 or 10x5 keys

# Embedded 42 keys keyboard (on T-COMPUTER and PICOMPUTER)

# USB keyboard (on T-COMPUTER and other Teensy 4.1 platforms)

# Installation
- Format the SD card as FAT32
- extract the content of SD.zip in the root directory 
- there must be a sub-directorie for each emulator
  - "2600" => for atarivcs, put your Atari 2600 roms here (.bin), with sub-dirs or not
  - "5200" => for atari5200, put your Atari 5200 roms here (.bin)
  - "800"  => for atari800, put your Atari 800 cartridges here (.rom)
  - 'c64"  => for c64, put your C64 programs here (.prg), with sub-dirs or not
  - "vic20"  => for VIC20, put your V20 programs (.prg), with sub-dirs or not 
  - "apple2"  => for Apple2 disks, put your ".dsk" files here, with sub-dirs or not 
  - "o2em" => for odyssey/videopac, put your Videopac/Odysssey roms here (.bin)
  - "coleco" => for colem, put your Colecovision roms here (.rom, including coleco.rom)
  - "nes"  => for nes/nofrendo, put your .nes files here (e.g. galaga,xevious,mario1...)
  - "spec" => for spectrum, put your ".z80" or ".sna" files here into sub-dirs or not ( max 48K )
  - "z81"  => for zx81, put your ".p", ".81"(, ".56") ".80" or ".o" files here ( max 56K ) 
  - "st"  => for AtariST, put your floppis ".st" files here
  - "pc"  => for 8086, put your HDD image ".img" 
  - "vectrex"  => for Vectrex, put your ".gam" files here
  - "pce"  => for PC Engine, put your ".pce" files here  
  - "gameboy"  => for Gameboy and GBColor, put your ".gb"/"gbc" files here  
  - "sms"  => for Sega Master System/Game Gear, put your ".sms"/"gg" files here  
  - "gen"  => for Sega Genesis/Megadrive, put your ".smd"/"bin" files here 
  - "msx"  => for MSX roms, put your ".rom"/".msx2" files here. MSX system roms in msx/roms 
  - "amiga"  => for Amiga floppies, put your ".adf" and ".hdf" files here.
  - "data"  => for Doom, put your WAD files there (doom1,doom2,plutonia or tnt)
  
- insert the card into the SD slot

# Compilation/flashing (Teensy)
- open the respective ino file with Arduino SDK
- select DISPLAY MODULE in platform_config.h (ST7789 or ILI9341)!!!!
- select VGA or not in platform_config.h (T4.1 using vgat4 lib, T3.6 using uvga lib)
- double check iopins.h for pins configuration!!!!
- compile and install from there.

# Compilation/flashing (ESP32)
- go to espboot directory first:
- make, make flash => this will install custom partition table, bootloader and app selector 
- go to any other directory (one by one!):
- Adapt ./flashapp.sh according your OS (I use OSX)
- make, ./flashapp.sh

# Compilation (PICO)
- git clone -b master https://github.com/raspberrypi/pico-sdk.git
- cd pico-sdk/
- git submodule update --init
- export PICO_SDK_PATH=path-to-pico-sdk
- Insure MCUME_pico is NEXT to pico-sdk directory
- go to MCUME_pico/build
- select emulator to compile in CMakeLists.txt (uncomment ${XXX_SOURCES} and comment out the rest)
- select platform options in config/platform_confih.h (e.g. PICOMPUTER, default is MCUME_REV1)
- cmake ..
- make
- drag and drop mcume.uf2 after having boouted the PICO os mass storage mode

# Status and known issues
- 64 (C64):
  - Full speed with sound
  - I2C custom keyboard support! 
  - (only few games working)
- colem (Colecovision):
  - Full speed with sound
- vcs (Atari 2600):
  - no sound yet
  - limited cartridge size support (due to ram constraint)
- o2em (Videopac/Odyssey):
  - no sound
  - only videopac G7000 games supported (due to ram constraint)
- nofrendo (Nintendo NES):
  - Full speed with sound
  - 32k roms only supported except on Teensy 4.0
- 5200 (Atari 5200):
  - Full speed with sound
  - 16 and 32k roms
- speccy (Zx spectrum):
  - SNA support (Z80 to be fixed)
  - YM and preliminary buzz sound support
  - 48k games only supported
  - kempston joystick supported but on screen keyboard may not answer in some games
  - I2C custom keyboard support!
- 81 (Zx81):
  - zx80 and zx81 
  - .P, .81 and .80 format support (rename .56 for Zx81 hires game that requires 48k or more)
  - I2C custom keyboard support!
  - HIRES support for zx81
  - zx80 support
  - YM sound support for zx81
- 800 (Atari 400/800/XE/XL):
  - .rom support (no floppy yet)
  - I2C custom keyboard support!
  - Full speed with sound
- Castaway (AtariST):
  - floppy support now with Read and Write access
  - I2C or integrated (TEECOMPUTER) keyboard support!
  - USB mouse and keyboard support (T4.1)
  - with sound
  - full speed on Teensy4.0 only!!
- 8086 (XT PC):
  - HDD image support
  - I2C custom keyboard support!
  - no sound
  - full speed on Teensy4.0 only!!
- Vectrex (MB Vectrex):
  - .gam roms support
  - no sound
  - full speed on Teensy4.0 only!!
- PCE (PC Engine):
  - .pce roms support
  - Nearly full speed with sound
  - Some games hang or do not start as Outrun, Powerdrift...
- SMS/GG (Sega Master System/Game Gear):
  - Full speed with sound
- Gameboy (and Game Boy Color):
  - Full speed with sound
  - Major missing background issue
- Genesis (Sega Megadrive):
  - Nearly Full speed with rarely sound
  - Some background  rotation issue
  - Some games do not start
- MSX1/2:
  - Full speed with sound
  - up to 256KB roms
  - some games are not working as uridium2
- Amiga:
  - Full speed with sound
  - up to 2MB chip RAM + 4MB expand fast RAM roms
  - HDF (Hard Disk) and adf support
  - USB mouse and keyboard support (T4.1)
  - Improved compatibility (still no AGA support!)
- Doom:
  - Full speed without sound
- Vic20:
  - Initial Audio
  - Compatibility issues (dont expect to run mario and other demos!)
- Apple2:
  - No Audio
  - Compatibility issues
  
# Running
- ESP32 only: Select the emulator from the app selector screen and press FIRE (wait a bit that the SD get mounted)
- For the launched emulator you should see the roms of the SD card being listed
- you can select the rom with up/down 
- you can start the game by pressing the FIRE key
- while the game is running, you might have to press USER1 or USER2 keys to start the game
- some emulators requires the I2Ckeyboard
- You can then play the game with the analog joystick and the FIRE/USER1 keys  
- press the USER1+USER2 to reboot the emulator and load another ROM
- USER2 can be used to swap joystick 1/2 in some emulators

# Running on the PICOMPUTER/T-COMPUTER
- Fire button is mapped to key 'TAB'
- USER2 is mapped to key "fn"
- USER1 is mapped to "fn+DEL"
- for PICO64, fb+DEL can be used to load selected game
- AUTORUN: keep fn key pressed until LED flashes. Launch game with fire. (to remove AUTORUN, press down key at reset)

# Credits
I mostly ported the emulators from existing projects, all the credit goes to the authors of
colem, o2em , x2600, moarnes, mc-4u, sz81, atari800, Vice, Nofrendo, jun52, dcastaway, vectrexemu (sorry if I don't mention their name), Charles Macdonald (TGemu), Bernd Schmidt (UAE), Author of Aiie, Vic20 emu and  Mike Chambers for his 8086 Arduino project!<br>
Doom is based on the ST32 port of chocolate doom so thanks for initial porting effort.<br>
Thanks a lot also to Frank Boesing for his ILI DMA library from which I started from and his great Teensy64 project https://github.com/FrankBoesing/Teensy64

