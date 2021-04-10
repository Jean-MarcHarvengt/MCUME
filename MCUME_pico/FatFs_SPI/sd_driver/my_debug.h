#pragma once

#include <stdio.h>

void my_printf(const char *pcFormat, ...) __attribute__((format(__printf__, 1, 2)));

//#if defined(DEBUG) && !defined(NDEBUG)
#define DBG_PRINTF my_printf
//#else
//#define DBG_PRINTF(fmt, args...) /* Don't do anything in release builds*/
//#endif

void my_assert_func(const char *file, int line, const char *func,
                    const char *pred);

#define myASSERT(__e) \
    ((__e) ? (void)0 : my_assert_func(__FILE__, __LINE__, __func__, #__e))
