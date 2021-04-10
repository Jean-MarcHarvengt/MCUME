#only need cmake, ARM cross-compiler and pico-sdk (no extra, no playground)
git clone -b master https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk/
git submodule update --init
export PICO_SDK_PATH=path-to-pico-sdk
cd path-to-MCUME-pico
# select emulator to compile in CMakeLists.txt, uncomment ${XXX_SOURCES} and comment out the rest
#add_executable(mcume
#		${GFXENGINE_SOURCES}
#)
cd build
cmake ..
make 

