#
# Component Makefile
#
# This Makefile can be left empty. By default, it will take the sources in the
# src/ directory, compile them and link them into lib(subdirectory_name).a
# in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#

COMPONENT_ADD_INCLUDEDIRS := . ../..
COMPONENT_SRCDIRS := . 

CPPFLAGS += -O3 -ffast-math -DUSE_FAME_CORE_C -DUSE_FAME_CORE -DFAME_GLOBAL_CONTEXT 
CPPFLAGS += -DNO_SOUND #-DEMULATED_JOYSTICK  -DUSE_SHORT_SLICE -DUSE_DOUBLE_BUFFER 
