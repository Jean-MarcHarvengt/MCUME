#
# Main component makefile.
#
# This Makefile can be left empty. By default, it will take the sources in the
# src/ directory, compile them and link them into lib(subdirectory_name).a
# in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#

CPPFLAGS += -Wno-error=pointer-sign -Wno-error=unused-function -Wno-error=implicit-function-declaration -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wno-unused-parameter -Wno-error=char-subscripts -Wno-error=attributes
#-Werror=maybe-uninitialized  