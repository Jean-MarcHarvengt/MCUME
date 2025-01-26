
# install PICO-SDK (or update it by pulling)
Use pico-sdk 2.0 (for RP2350, pico2)
------------------------------------
git clone -b master https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk/
git submodule update --init
cd lib/tinyusb
python3 tools/get_deps.py rp2040
python3 tools/get_deps.py rp2350
cd ..
export PICO_SDK_PATH=/Users/jean-marcharvengt/Documents/pico/pico-sdk-2.1.0 (path to pico-sdk!)


mkdir build
cd build
pico2/2w: cmake -DPICO_PLATFORM=rp2350 -DPICO_BOARD=pico2 ..

make
