

#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

//#include <stddef.h>
#include "stdlib.h"

void *osi_malloc(size_t size);
void *osi_calloc(size_t size);
void osi_free(void *ptr);
void osi_freeAndSetNull(void **ptr);
void osi_srand(unsigned int init);
int osi_rand(void);

#endif /* _ALLOCATOR_H_ */
