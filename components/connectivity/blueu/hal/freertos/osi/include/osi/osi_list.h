#ifndef _OSI_LIST_H_
#define _OSI_LIST_H_

struct list_node_t;
typedef struct list_node_t list_node_t;

struct list_t;
//typedef struct list_t list_t;

struct list_node_t
{
    struct list_node_t *next;
    void *data;
};

typedef struct list_t
{
    list_node_t *head;
    list_node_t *tail;
    size_t length;
} list_t;

typedef int (*list_iter_cb)(void *data, void *context);

list_t *list_new(void);
list_node_t *list_free_node(list_t *list, list_node_t *node);
void list_free(list_t *list);
int list_is_empty(const list_t *list);
int list_contains(const list_t *list, const void *data);
size_t list_length(const list_t *list);
void *list_front(const list_t *list);
void *list_back(const list_t *list);
list_node_t *list_back_node(const list_t *list);
int list_insert_after(list_t *list, list_node_t *prev_node, void *data);
int list_prepend(list_t *list, void *data);
int list_append(list_t *list, void *data);
int list_remove(list_t *list, void *data);
void list_clear(list_t *list);
list_node_t *list_foreach(const list_t *list, list_iter_cb callback, void *context);
list_node_t *list_begin(const list_t *list);
list_node_t *list_end(const list_t *list);
list_node_t *list_next(const list_node_t *node);
void *list_node(const list_node_t *node);
void list_free(list_t *list);

#endif
