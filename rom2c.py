
# loads a zx rom and dumps it into a C array

rom_file = "spectrum-roms/48.rom"
name = "zxspectrum48rom"
code_file = f"MCUME_pico/picospeccy/{name}.inl"

with open(rom_file, "rb") as ifd, open(code_file, "w") as ofd:
  rom = [str(b) for b in ifd.read()]
  #print(int.from_bytes(rom[0], "little"))
  ofd.write(f"unsigned char {name}[] = {{{','.join(rom)}}};\n")
