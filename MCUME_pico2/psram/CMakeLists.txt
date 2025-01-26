# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)

project(rp2040-psram
    VERSION 1.0.0
    DESCRIPTION "A header-only library to allow access to SPI PSRAM via PIO on the RP2040 microcontroller."
)

add_library(rp2040-psram INTERFACE)

target_sources(rp2040-psram INTERFACE
        psram_spi.c
)
target_include_directories(rp2040-psram INTERFACE ${CMAKE_CURRENT_LIST_DIR})

if(PICO_SDK)
    pico_generate_pio_header(rp2040-psram ${CMAKE_CURRENT_LIST_DIR}/psram_spi.pio)
endif(PICO_SDK)

option(BUILD_PSRAM_DOCS "Build documentation" OFF)

if(BUILD_PSRAM_DOCS)
    # check if Doxygen is installed
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        # note the option ALL which allows to build the docs together with the application
        add_custom_target( doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    else (DOXYGEN_FOUND)
      message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif(BUILD_PSRAM_DOCS)
