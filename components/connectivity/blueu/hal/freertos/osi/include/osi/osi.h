#ifndef __OSI_OSI_H__
#define __OSI_OSI_H__

#include <string.h>
#include <stddef.h>
#include <assert.h>
#include "bt_types.h"
#include "osi_allocator.h"
#include "osi_mutex.h"
#include "osi_timer.h"
#include "osi_thread.h"
#include "osi_mempool.h"
#include "osi_utils.h"


#ifndef UNUSED
#define UNUSED(x)   (void)(x)
#endif


int osi_init(void);
int osi_reset(void);

#define osi_assert(__e) assert(__e)

//#define osi_assert(var)  assert(var);

#endif /* __OSI_OSI_H__ */

