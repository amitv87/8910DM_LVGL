#ifndef _LINKLIST_H
#define _LINKLIST_H

#include "quec_common.h"

typedef enum
{
    DIR_FIFO,
    DIR_LIFO
} push_dir_t;

#define DIR_DEFAULT DIR_FIFO
#define FALSE 0
typedef unsigned char data_t;

typedef uint datasize_t;

typedef uint node_cnt_t;

typedef int  linklist_id_t;

#pragma pack(push)
#pragma pack(1)
typedef struct node
{
	struct node * next;
    datasize_t size;
    data_t data[1];
} linklist;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
typedef struct
{
    node_cnt_t node_cnt;
    linklist * header;
    linklist * tail;
    linklist * curt;
} linklist_t;
#pragma pack(pop)

typedef linklist_t * linklist_handler_t;

extern bool 		bLinklistLogEn;

extern QuecOSStatus linklist_init(linklist_handler_t * linklist_handler);
extern QuecOSStatus linklist_push(linklist_handler_t * linklist_handler, push_dir_t dir, data_t * data, datasize_t size);
//extern QuecOSStatus linklist_pop(linklist_handler_t * linklist_handler, data_t * * data_pp, datasize_t * data_len);
extern QuecOSStatus linklist_get_node_cnt(linklist_handler_t * linklist_handler, node_cnt_t * nodeCnt);
extern QuecOSStatus linklist_node_find(linklist_handler_t * linklist_handler, uint32_t i, linklist * * dest_node);
extern QuecOSStatus linklist_node_delete(linklist_handler_t * linklist_handler, uint32_t i);
extern QuecOSStatus linklist_clean(linklist_handler_t * linklist_handler);
extern QuecOSStatus linklist_deinit(linklist_handler_t * linklist_handler);
extern void 		linklist_mem_free(void * mem_ptr);

#endif

