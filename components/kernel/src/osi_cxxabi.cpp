/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#include "osi_api.h"
#include <cstdlib>
#include <new>

namespace std
{
void __throw_out_of_range(const char *)
{
    osiPanic();
}

void __throw_length_error(const char *)
{
    osiPanic();
}
} // namespace std

void *operator new(size_t size) noexcept
{
    return malloc(size);
}

void operator delete(void *ptr) noexcept
{
    free(ptr);
}

void *operator new[](size_t size) noexcept
{
    return malloc(size);
}

void operator delete[](void *ptr) noexcept
{
    free(ptr);
}

void *operator new(size_t size, std::nothrow_t) noexcept
{
    return malloc(size);
}

void operator delete(void *ptr, std::nothrow_t) noexcept
{
    free(ptr);
}

void *operator new[](size_t size, std::nothrow_t) noexcept
{
    return malloc(size);
}

void operator delete[](void *ptr, std::nothrow_t) noexcept
{
    free(ptr);
}

extern "C" void __cxa_pure_virtual()
{
    osiPanic();
}

extern "C" void __register_exitproc()
{
}

extern "C" int __aeabi_atexit(void *object, void (*dtor)(void *), void *dso_handler)
{
    return 0;
}

void *__dso_handle = nullptr;

extern "C" void osiInvokeGlobalCtors(void)
{
    extern void (*__init_array_start[])();
    extern void (*__init_array_end[])();

    size_t count = __init_array_end - __init_array_start;
    for (size_t i = 0; i < count; ++i)
        __init_array_start[i]();
}
