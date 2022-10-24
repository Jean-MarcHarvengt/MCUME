# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/afs/inf.ed.ac.uk/user/s19/s1970742/Desktop/hons/MCUME/pico-sdk/tools/pioasm"
  "/afs/inf.ed.ac.uk/user/s19/s1970742/Desktop/hons/MCUME/MCUME_pico/cmake-build-debug/pioasm"
  "/afs/inf.ed.ac.uk/user/s19/s1970742/Desktop/hons/MCUME/MCUME_pico/cmake-build-debug/pico-sdk/src/rp2_common/cyw43_driver/pioasm"
  "/afs/inf.ed.ac.uk/user/s19/s1970742/Desktop/hons/MCUME/MCUME_pico/cmake-build-debug/pico-sdk/src/rp2_common/cyw43_driver/pioasm/tmp"
  "/afs/inf.ed.ac.uk/user/s19/s1970742/Desktop/hons/MCUME/MCUME_pico/cmake-build-debug/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src/PioasmBuild-stamp"
  "/afs/inf.ed.ac.uk/user/s19/s1970742/Desktop/hons/MCUME/MCUME_pico/cmake-build-debug/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src"
  "/afs/inf.ed.ac.uk/user/s19/s1970742/Desktop/hons/MCUME/MCUME_pico/cmake-build-debug/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src/PioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/afs/inf.ed.ac.uk/user/s19/s1970742/Desktop/hons/MCUME/MCUME_pico/cmake-build-debug/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src/PioasmBuild-stamp/${subDir}")
endforeach()
