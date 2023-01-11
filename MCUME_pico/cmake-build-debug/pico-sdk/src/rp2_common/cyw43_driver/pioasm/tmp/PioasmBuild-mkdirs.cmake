# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/alint/Desktop/eu/uni/_year4/Dizertatie/MCUME/pico-sdk/tools/pioasm"
  "C:/Users/alint/Desktop/eu/uni/_year4/Dizertatie/MCUME/MCUME_pico/cmake-build-debug/pioasm"
  "C:/Users/alint/Desktop/eu/uni/_year4/Dizertatie/MCUME/MCUME_pico/cmake-build-debug/pico-sdk/src/rp2_common/cyw43_driver/pioasm"
  "C:/Users/alint/Desktop/eu/uni/_year4/Dizertatie/MCUME/MCUME_pico/cmake-build-debug/pico-sdk/src/rp2_common/cyw43_driver/pioasm/tmp"
  "C:/Users/alint/Desktop/eu/uni/_year4/Dizertatie/MCUME/MCUME_pico/cmake-build-debug/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src/PioasmBuild-stamp"
  "C:/Users/alint/Desktop/eu/uni/_year4/Dizertatie/MCUME/MCUME_pico/cmake-build-debug/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src"
  "C:/Users/alint/Desktop/eu/uni/_year4/Dizertatie/MCUME/MCUME_pico/cmake-build-debug/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src/PioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/alint/Desktop/eu/uni/_year4/Dizertatie/MCUME/MCUME_pico/cmake-build-debug/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src/PioasmBuild-stamp/${subDir}")
endforeach()
