# M.CU.M.E = Multi CompUter Machine Emulator
<p align="center">
  <img width="320" height="140" src="/images/logo.png">
</p>

# News
April 2021: Add initial support for raspberry PICO<br>
- adapted pico scanline library for  VGA 8 bits mode (RRRGGGBB), sorry adapted code is embedded...
- SD support from https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico (fixed IRQ to 1)
- rudimentary PWM IRQ based audio (https://gregchadwick.co.uk/blog/playing-with-the-pico-pt3/)

Far from perfect, lot of bugs, and ... unfortunately C64 emulator has no sound for performance reason!<br>
All what you need is in MCUME_pico. Precompiled binaries in MCUME_pico/bin.<br>
To recompile, look at MCUME_pico/readme.txt<br><br>

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
A collection of 80th game Console and Computer emulators ported to Teensy and ESP32 MCUs.<br>
Have a look at the video:
https://youtu.be/jlqoWH2X6gk<br><br>
This project is a spin-off of the TeensyCEC project https://github.com/Jean-MarcHarvengt/TeensyCEC<br>
Now ported to much more MCUs! <br>

# Featuring

Computer systems supported and status on various MCU platforms<br>

| System | Teensy3.6 | Teensy 4.0 | Teensy4.0 +PSRAM | Teensy4.1 +PSRAM+VGA | ESP32 | ESP32-Wrover | Pico |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Zx81        | X | X | X | X | X | X | X |
| Zx spectrum | X | X | X | X | X | X | X |
| Atari 800   | X | X | X | X | X | X | X |
| C64         | X | X | X | X | X | X | X |
| Atari 520ST | - | full speed! | X | X (640x400!) | - | slow | - |
| 8086 XT PC  | - | full speed! | X | X | - | slow | - |
| MSX1/2      | - | full speed! | X | X | - | - | - |
| Amiga       | - | - | exp only! | X (640x240!) | - | - | - |
| Doom        | - | - | - | x | - | - | - |

Please compile for smallest code on the Teensy4.0 else you will run out of memory<br><br>
For Teensy 4.1 with PSRAM try compiling for faster code (unless it complains at compilation).<br> 
Doom for example only works if compiled for faster code!

Game console systems supported and status on various MCU platforms<br>

| System | Teensy3.6 | Teensy 4.0 | Teensy4 +PSRAM | Teensy4.1 +PSRAM+VGA | ESP32 | ESP32-Wrover | Pico |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Atari 2600                   | X | X | X | X | X | X | X |
| Odyssey/Videopac             | X | X | X | X | X | X | X |
| Colecovision                 | X | X | X | X | X | X | X |
| Atari 5200                   | X | X | X | X | X | X | X |
| Nintendo NES                 | X | big roms! | X | X | X | X | - |
| Vectrex                      | - | full speed! | X | X | - | slow | - |
| PC Engine                    | - | - | X | X | - | - | - |
| Gameboy/GBColor              | - | - | X | X | - | - | - |
| Sega Master System/Game Gear | - | - | X | X | - | - | - |
| Sega Genesis/Megadrive       | - | - | X | X | - | - | - |

# Minimal requirements:
- Teensy3.6/Teensy4.0, ESP32 Node32S/Wrover SDK chip or Raspberry PICO
- ILI9341 (Teensy/ESP32) or ST7789 (Teensy only) SPI display
- SD card (Teensy uses built-in uSD, ESP32 uses external one connected over SPI, e.g. ILI9341 integrated one)
- Analog joypad (Arduino or PSP like)
- 3 buttons (FIRE, USER1 and USER2)

# Optional requirements:
- I2C custom keyboard (for computers emulators)
- Sound (DAC for ESP and Teensy3.6, MQS for Teensy4.0)
- VGA output (Teensy 3.6 and Teensy 4.1 only)
- 2 extra buttons (USER3 and USER4) mostly for NES emu

# Wiring
- see pinout.txt file in the respective project (ESP32 or Teensy)
- Some KICAD image also available for ESP32

# I2C keyboard (for computer emus)
- see i2ckeyboard sub-directory
- the I2C keyboard is using a separate atmega328p MCU handling the keys matrix
- with 10x4 or 10x5 keys

# Installation
- Format the SD card as FAT32
- extract the content of SD.zip in the root directory 
- there must be a sub-directorie for each emulator
  - "2600" => for atarivcs, put your Atari 2600 roms here (.bin), with sub-dirs or not
  - "5200" => for atari5200, put your Atari 5200 roms here (.bin)
  - "800"  => for atari800, put your Atari 800 cartridges here (.rom)
  - 'c64"  => for c64, put your C64 programs here (.prg), with sub-dirs or not
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
  
-- insert the card into the SD slot

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
  - floppy support
  - I2C custom keyboard support!
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

# Credits
I mostly ported the emulators from existing projects, all the credit goes to the authors of
colem, o2em , x2600, moarnes, mc-4u, sz81, atari800, Vice, Nofrendo, jun52, dcastaway, vectrexemu (sorry if I don't mention their name), Charles Macdonald (TGemu), Bernd Schmidt (UAE) and  Mike Chambers for his 8086 Arduino project!<br>
Doom is based on the ST32 port of chocolate doom so thanks for initial porting effort.<br>
Thanks a lot also to Frank Boesing for his ILI DMA library from which I started from and his great Teensy64 project https://github.com/FrankBoesing/Teensy64

