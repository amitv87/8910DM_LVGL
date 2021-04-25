/*
 *  <blueu file> - <bluetooth stack implementation>
 *
 *  Copyright (C) 2019 Unisoc Communications Inc.
 *  History:
 *      <2019.10.11> <wcn bt>
 *      Description
 */

#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "osi_allocator.h"
#include "log.h"

static unsigned int random_value = 1;

void *osi_malloc(size_t size)
{
//    void *p = pvPortMalloc(size);
#if 1  // 4byte align
    size_t temp = ((size & 0x3) == 0)?(0):((4 - (size & 0x3)));
    size_t alloc_size = temp + size;
#endif

    void *p = malloc(alloc_size);
    if(p)
    {
        memset(p, 0, alloc_size);
    }
    
//    OSI_LOGI(0,"BT malloc:0x%x", ((unsigned int)p));
    
    return p;
}

void *osi_calloc(size_t size)
{
//    return pvPortMalloc(size);
#if 1  // 4byte align
    size_t temp = ((size & 0x3) == 0)?(0):((4 - (size & 0x3)));
    size_t alloc_size = temp + size;
#endif

    return malloc(alloc_size);
}


void osi_free(void *ptr)
{
//    vPortFree(ptr);
//    OSI_LOGI(0,"BT free:0x%x", ((unsigned int)ptr));
    free(ptr);
}

/*----------------------------------------------------------------------------*
 *  NAME
 *      OS_PfreeAndSetNull
 *
 *  DESCRIPTION
 *      free memory and set null
 *
 *  RETURNS
 *      void
 *
 *----------------------------------------------------------------------------*/
void osi_freeAndSetNull(void **ptr)
{
//    OSI_LOGI(0,"BT free:0x%x", ((unsigned int)*ptr));
    
    free(*ptr);
    *ptr = (void*)0;

    return;
}

void osi_srand(unsigned int init)
{
    random_value = init;
}

int osi_rand(void)
{
    random_value = random_value * 1103515245 + 12345;
    return (unsigned int)(random_value / 131072) % 65536;
}
