#./avrdude -C avrdude.conf  -P /dev/cu.usbmodemFD1441 -c stk500 -p m328p -U flash:w:prog.hex
./avrdude -C avrdude.conf  -P /dev/cu.usbmodemFA1221 -c stk500 -p m328p -U flash:w:keybrev.hex
