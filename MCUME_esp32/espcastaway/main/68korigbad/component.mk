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

CPPFLAGS += -fpermissive #-O3  -ffast-math  
CPPFLAGS += -DNO_SOUND #-DEMULATED_JOYSTICK  -DUSE_SHORT_SLICE -DUSE_DOUBLE_BUFFER 
CPPFLAGS += -Wno-error=sequence-point -Wno-error=parentheses -Wno-error=maybe-uninitialized  -Wno-unused-variable -fno-common -fno-builtin -fno-exceptions -fstrict-aliasing -Wno-narrowing

#CPPFLAGS += -Wno-error=parentheses -Wno-error=maybe-uninitialized -Wno-error=char-subscripts -Wno-error=attributes
#CPPFLAGS += -fno-common -fno-builtin -fno-exceptions -fstrict-aliasing -Wno-narrowing -Wno-error=maybe-uninitialized -Wno-error=sequence-point -Wno-error=implicit-int -Wno-error=pointer-sign -Wno-error=unused-function -Wno-error=implicit-function-declaration -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wno-unused-parameter -Wno-error=char-subscripts -Wno-error=attributes

