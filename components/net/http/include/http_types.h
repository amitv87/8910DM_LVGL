#ifndef _CG_HTTP_TYPES_H_
#define _CG_HTTP_TYPES_H_

typedef enum
{

    DOWNLOAD_FAIL = 1,
    DOWNLOAD_SUCCESS = 2,
    GETN_SUCCESS = 3,
    GETN_FAIL = 4,
    PUT_FAIL = 5,
    PUT_SUCCESS = 6,
    DELETE_FAIL = 7,
    DELETE_SUCCESS = 8,
    POSTN_FAIL = 9,
    POSTN_SUCCESS = 10,
    ACTIVE_FAIL = 11,
    ACTIVE_SUCCESS = 12,

} HTTP_status;

typedef struct
{

    HTTP_status status;

    void *msg;

} http_t;

typedef void (*packet_handler_t)(const unsigned char *data, size_t len);
typedef void (*memory_handler_t)(void *http_info);
typedef void (*status_handler_t)(long data_finish, long data_remain);
typedef void (*data_handler_t)(char *content, long content_length);
typedef void (*http_aworker_handler)(http_t download_msg);
typedef void (*http_DL_process_handler)(char *data, int total, int sum, int cur);

#endif
