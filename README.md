# M.CU.M.E = Multi CompUter Machine Emulator
<p align="center">
  <img width="320" height="140" src="/images/logo.png">
</p>

# News

Nov 2019: Gameboy, Sega Master System/ GameGear and Megadrive/Genesis emulator running with extra 8MB SPI RAM (PSRAM) <br>

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

| System | Teensy3.6 | Teensy 4.0 | Teensy4+PSRAM | ESP32 | ESP32-Wrover |
| --- | --- |--- | --- | --- | --- |
| Zx81        | X | X | X | X | X |
| Zx spectrum | X | X | X | X | X |
| Atari 800   | X | X | X | X | X |
| C64         | X | X | X | X | X |
| Atari 520ST | - | full speed! | X | - | slow |
| 8086 XT PC  | - | full speed! | X | - | slow |

Please compile for smallest code on the Teensy4.0 else you will run out of memory<br><br>

Game console systems supported and status on various MCU platforms<br>

| System | Teensy3.6 | Teensy 4.0 | Teensy4+PSRAM | ESP32 | ESP32-Wrover |
| --- | --- |--- | --- | --- | --- |
| Atari 2600                   | X | X | X | X | X |
| Odyssey/Videopac             | X | X | X | X | X |
| Colecovision                 | X | X | X | X | X |
| Atari 5200                   | X | X | X | X | X |
| Nintendo NES                 | X | big roms! | X | X | X |
| Vectrex                      | - | full speed! | X | - | slow |
| PC Engine                    | - | - | X | - | - |
| Gameboy/GBColor              | - | - | X | - | - |
| Sega Master System/Game Gear | - | - | X | - | - |
| Sega Genesis/Megadrive       | - | - | X | - | - |

# Minimal requirements:
- Teensy3.6/Teensy4.0 or ESP32 Node32S/Wrover SDK chip
- ILI9341 (Teensy/ESP32) or ST7789 (Teensy only) SPI display
- SD card (Teensy uses built-in uSD, ESP32 uses external one connected over SPI, e.g. ILI9341 integrated one)
- Analog joypad (Arduino or PSP like)
- 3 buttons (FIRE, USER1 and USER2)

# Optional requirements:
- I2C custom keyboard (for computers emulators)
- Sound (DAC for ESP and Teensy3.6, MQS for Teensy4.0)
- VGA output (Teensy 3.6 only)
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
-- insert the card into the SD slot

# Compilation/flashing (Teensy)
- open the respective ino file with Arduino SDK
- select DISPLAY MODULE in tft_t_dma_config.h (ST7789 or ILI9341)!!!!
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
colem, o2em , x2600, moarnes, mc-4u, sz81, atari800, Vice, Nofrendo, jun52, dcastaway, vectrexemu (sorry if I don't mention their name), Charles Macdonald (TGemu) and  Mike Chambers for his 8086 Arduino project!
Thanks a lot also to Frank Boesing for his ILI DMA library from which I started from and his great Teensy64 project https://github.com/FrankBoesing/Teensy64

