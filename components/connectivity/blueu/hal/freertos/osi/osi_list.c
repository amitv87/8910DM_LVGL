#include <stdlib.h>
#include <stdio.h>
#include "osi_list.h"
#include "osi_allocator.h"
#include "bt_types.h"

#define BT_LIST_NODE_NUM 100
struct bt_list_node_memory_ctrl
{
    list_node_t node_mem[BT_LIST_NODE_NUM];
    list_node_t *free_head;
};

struct bt_list_node_memory_ctrl  g_bt_list_node_memory_ctrl;

void list_node_init(void)
{
    list_node_t *ptr;
    
    g_bt_list_node_memory_ctrl.free_head = &(g_bt_list_node_memory_ctrl.node_mem[0]);
    ptr = g_bt_list_node_memory_ctrl.free_head;

    for(UINT8 i = 1; i < BT_LIST_NODE_NUM; i++)
    {
        ptr->next = &(g_bt_list_node_memory_ctrl.node_mem[i]);
        ptr = ptr->next;
    }

    ptr->next = NULL;
}

static void* list_node_alloc(void)
{
    if(g_bt_list_node_memory_ctrl.free_head == NULL)
    {
        return NULL;
    }

    list_node_t *ptr = g_bt_list_node_memory_ctrl.free_head;
    g_bt_list_node_memory_ctrl.free_head = ptr->next;

    ptr->next = NULL;

    return ptr;
}

static void list_node_free(struct list_node_t *ptr)
{
    ptr->next = g_bt_list_node_memory_ctrl.free_head;

    g_bt_list_node_memory_ctrl.free_head = ptr;
}


list_t *list_new(void)
{
    list_t *list = (list_t *)osi_malloc(sizeof(list_t));

    if (!list)
    {
        return NULL;
    }

    list->head = list->tail = NULL;
    list->length = 0;
    return list;
}

void list_free(list_t *list)
{
    if (!list)
    {
        return;
    }

    list_clear(list);
}
int list_is_empty(const list_t *list)
{
    return (list->length == 0);
}

int list_contains(const list_t *list, const void *data)
{
    for (const list_node_t *node = list_begin(list); node != list_end(list); node = list_next(node))
    {
        if (list_node(node) == data)
        {
            return true;
        }
    }

    return false;
}

size_t list_length(const list_t *list)
{
    return list->length;
}

void *list_front(const list_t *list)
{
    return list->head->data;
}

void *list_back(const list_t *list)
{
    return list->tail->data;
}

list_node_t *list_back_node(const list_t *list)
{
    return list->tail;
}

int list_insert_after(list_t *list, list_node_t *prev_node, void *data)
{
    list_node_t *node = (list_node_t *)osi_malloc(sizeof(list_node_t));

    if (!node)
    {
        return false;
    }

    node->next = prev_node->next;
    node->data = data;
    prev_node->next = node;

    if (list->tail == prev_node)
    {
        list->tail = node;
    }

    ++list->length;
    return true;
}

int list_append(list_t *list, void *data)
{
#if 0
    list_node_t *node = (list_node_t *)osi_malloc(sizeof(list_node_t));
#else
    list_node_t *node = (list_node_t *)list_node_alloc();
#endif

    if (!node)
    {
        return false;
    }

    node->next = NULL;
    node->data = data;

    if (list->tail == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }

    ++list->length;
    return true;
}

int list_remove(list_t *list, void *data)
{
    list_node_t *prev;
    list_node_t *node;
    
    if (list_is_empty(list))
    {
        return false;
    }

    if (list->head->data == data)
    {
        list_node_t *next = list_free_node(list, list->head);

        if (list->tail == list->head)
        {
            list->tail = next;
        }

        list->head = next;
        return true;
    }

    for (prev = list->head, node = list->head->next; node; prev = node, node = node->next)
    {
        if (node->data == data)
        {
            prev->next = list_free_node(list, node);

            if (list->tail == node)
            {
                list->tail = prev;
            }

            return true;
        }
    }

    return false;
}

void list_clear(list_t *list)
{
    for (list_node_t *node = list->head; node; )
    {
        node = list_free_node(list, node);
    }

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
}

list_node_t *list_foreach(const list_t *list, list_iter_cb callback, void *context)
{
    for (list_node_t *node = list->head; node; )
    {
        list_node_t *next = node->next;

        if (!callback(node->data, context))
        {
            return node;
        }

        node = next;
    }

    return NULL;
}

list_node_t *list_begin(const list_t *list)
{
    return list->head;
}

list_node_t *list_end(const list_t *list)
{
    return NULL;
}

list_node_t *list_next(const list_node_t *node)
{
    return node->next;
}

void *list_node(const list_node_t *node)
{
    return node->data;
}

list_node_t *list_free_node(list_t *list, list_node_t *node)
{
    list_node_t *next = node->next;
#if 0
    osi_free(node);
#else
    list_node_free(node);
#endif
    --list->length;

    return next;
}
