# M.CU.M.E = Multi CompUter Machine Emulator
<p align="center">
  <img width="320" height="140" src="/images/logo.png">
</p>
<p align="center">
<img src="/images/esp321.jpg" width="200" />  
<img src="/images/esp322.jpg" width="200" />  
</p>
A collection of 80th game Console and Computer emulators for the ESP32 MCU.<br>
Have a look at the video:
https://youtu.be/jlqoWH2X6gk<br><br>
This project is a spin-off of the TeensyCEC project https://github.com/Jean-MarcHarvengt/TeensyCEC<br>
I plan to bring back the Teensy3.6 code under the umbrella of this project soon<br>
My goal is that M.CU.M.E get ported on multiple MCU platforms, to be able to expand it with more!<br>

# Featuring
5 game consoles:
Atari2600 (espvcs), Philips Videopac/Odyssey (espo2em), Colecovision (espcolem), NES (espnofrendo) and Atari5200 (esp5200).<br>
4 computer cores:
Zx80/Zx81 (esp81), Zx Spectrum (espspeccy), Atari800 (esp800)and C64 (esp64)

# Minimal requirements:
- ESP32 Node32S sdk chip
- ILI9341 display with SD card (2.2/2.8 inches version)
- Analog joypad (Arduino or PSP like)
- 3 buttons (FIRE, USER1 and USER2)

# Optional requirements:
- I2C custom keyboard (for computers emulators) 

# Wiring
- see pinout.txt file in the project

# I2C keyboard (best to have)
- see pinout.txt file in i2ckeyboard sub-directory
- the I2C keyboard is using a separate atmega328p MCU handling the keys matrix

# Installation
- Format the SD card as FAT32
- extract the content of SD.zip in the root directory 
- there must be a sub-directorie for each emulator
  - "2600" => for atarivcs, put your Atari 2600 roms here (.bin)
  - "5200" => for atari5200, put your Atari 5200 roms here (.bin)
  - "800"  => for atari800, put your Atari 800 cartridges here (.rom)
  - 'c64"  => for c64, put your C64 programs here (.prg)
  - "o2em" => for odyssey/videopac, put your Videopac/Odysssey roms here (.bin)
  - "coleco" => for colem, put your Colecovision roms here (.rom, including coleco.rom)
  - "nes"  => for nes/nofrendo, put your .nes files here, onlt 32k games are supported (galaga,xevious,mario1...)
  - "spec" => for spectrum, put your ".z80" or ".sna" files here into sub-dirs or not ( max 48K )
  - "z81"  => for zx81, put your ".p", ".81"(, ".56") ".80" or ".o" files here, into sub-dirs or not ( max 56K ) 
- insert the SD into the ILI9341 SD slot

# Compilation/flashing
- go to espboot directory first:
- make, make flash => this will install custom partition table, bootloader and app selector 
- go to any other directory (one by one!):
- Adapt ./flashapp.sh according your OS (I use OSX)
- make, ./flashapp.sh

# Status and known issues
- 64:
  - with sound
  - I2C custom keyboard support! 
  - Many games crashe
- colem:
  - with sound!
- vcs:
  - no sound yet
  - limited cartridge size support (due to ram constraint)
- o2em:
  - no sound
  - only videopac G7000 games supported (due to ram constraint)
- nofrendo:
  - Full speend with sound
  - 32k roms only supported
- 5200:
  - with sound
  - 16 and 32k roms
- speccy:
  - SNA support (Z80 to be fixed)
  - YM and preliminary buzz sound support
  - 48k games only supported
  - kempston joystick supported but on screen keyboard may not answer in some games
  - I2C custom keyboard support!
- 81:
  - zx80 and zx81 
  - .P, .81 and .80 format support (rename .56 for Zx81 hires game that requires 48k or more)
  - I2C custom keyboard support!
  - HIRES support for zx81
  - zx80 support
  - YM sound support for zx81
- 800:
  - .rom support (no floppy yet)
  - I2C custom keyboard support!
  - with sound
  
# Running
- Select the emulator from the app selector screen and press FIRE (wait a bit that the SD get mounted)
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
colem, o2em , x2600, moarnes, mc-4u, sz81, atari800, Vice, Nofrendo and jun52 projects!
Thanks a lot also to Frank Boesing for his ILI DMA library from which I started from and his great Teensy64 project https://github.com/FrankBoesing/Teensy64

