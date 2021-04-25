#ifndef __ASYNC_WORKER__
#define __ASYNCE_WORKER__
#include <stdlib.h>
#include "osi_api.h"
//#include "event.h"

typedef enum aworker_result_code
{
    AWRC_FAIL = -1,
    AWRC_SUCCESS = 0,
    AWRC_PROCESSING = 9876543,
} AWORKER_RC;

struct aworker_request;
typedef struct aworker_request AWORKER_REQ;
/**
*   handler return value include AWRC_FAIL, AWRC_SUCCESS, AWRC_PROCESSING,
*   AWRC_FAIL, AWRC_SUCCESS means handler complete and give the handled result;
*   AWRC_PROCESSING means handler was not completed, such as do asynchrous action
*   in this case, it must call aworker_post_result to post result
*/
typedef AWORKER_RC (*AWORKER_HANDLER)(AWORKER_REQ *req);
typedef void (*AWORKER_CALLBACK)(int result, uint32_t event, void *param);

struct aworker_request
{
    osiThread_t *sender; // sender task handle
    uint32_t event;
    void *param;
    AWORKER_HANDLER handler;
    AWORKER_CALLBACK callback;
};

bool aworker_start();
bool aworker_timer_callback(osiCallback_t tm_cb, uint32_t delay_ms, void *arg);
bool aworker_task_callback(osiCallback_t tcb, void *arg);
bool aworker_post_req_delay(AWORKER_REQ *req, uint32_t delay_ms, bool *result);
bool aworker_post_handler(AWORKER_HANDLER handler, void *param, uint32_t delay_ms);
bool aworker_post_result(AWORKER_REQ *req, int result);
AWORKER_REQ *aworker_create_request(osiThread_t *sender, AWORKER_HANDLER handler, AWORKER_CALLBACK callback,
                                    uint32_t event, void *param, uint32_t bufflen);
void aworker_dismiss_request(AWORKER_REQ *req);
void aworker_param_putu32(AWORKER_REQ *req, uint32_t val, bool *result);
void aworker_param_putbytes(AWORKER_REQ *req, uint8_t *barray, uint16_t len, bool *result);
void aworker_param_putstr(AWORKER_REQ *req, char *str, bool *result);
uint32_t aworker_param_getu32(AWORKER_REQ *req, uint8_t index, bool *result);
char *aworker_param_getstr(AWORKER_REQ *req, uint8_t index, bool *result);
uint16_t aworker_param_getbytes(AWORKER_REQ *req, uint8_t index, uint8_t *buffer, uint16_t bufflen, bool *result);
void aworker_dump_bytes(uint8_t *bytes, uint16_t len);

#endif // __ASYNCE_WORKER__
