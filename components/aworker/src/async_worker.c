#include "osi_log.h"
#include "async_worker.h"
#include "string.h"
#include "stdio.h"

typedef struct aworker_rsp
{
    int32_t result;
    uint32_t event;
    void *param;
    AWORKER_CALLBACK callback;
} AWORKER_RSP;

typedef enum aworker_task_state
{
    AWTS_STOPPED = 0,
    AWTS_STARTED,
} AWORKER_STATE;

typedef struct aworker_ctx
{
    osiThread_t *tid;
    bool runnable;
    uint8_t state;
    osiTimer_t *timerID;
    osiTimer_t *postDelayTimerID;
} AWORKER_CTX;

typedef struct aworker_req_int
{
    AWORKER_REQ req;
    uint8_t *buffer;
    uint32_t buffer_len;
    uint32_t buffer_used;
    uint16_t param_count;
    uint8_t last_index;
    uint8_t *last_ptr;
} AWORKER_REQ_INT;

typedef enum aworker_param_type
{
    AWPT_STRING = 1,
    AWPT_UINT32,
    AWPT_BYTES,
} AWORKER_TYPE;

typedef struct aworker_param_format
{
    uint8_t type;
    uint16_t len;
} AWORKER_PARAM_FORMAT;

#define AWORKER_DEF_BUFFER_LEN 512

#define AWORKLOG(args...)    \
    do                       \
    {                        \
        OSI_LOGI(0x0, args); \
                             \
    } while (0)

#define AWORKDBG(args...)

#define AWORKER_STACK_SIZE (2048 * 4)
// worker priority should be lower than AT task's,
// the smaller number means the lower priority
#define AWORKER_PRIORITY (OSI_PRIORITY_BELOW_NORMAL)
#define AWORKER_EVENT_QUEUE_SIZE (32)

static AWORKER_CTX s_aworker_ctx = {0};

static void aworker_init()
{
    memset(&s_aworker_ctx, 0, sizeof(AWORKER_CTX));
}

static void aworker_deinit()
{
    AWORKLOG("Enter aworker_deinit...");
    s_aworker_ctx.state = AWTS_STOPPED;
}

void aworker_dismiss_request(AWORKER_REQ *req)
{
    free(req);
}

static void aworker_handle_request(AWORKER_REQ *req)
{
    AWORKER_RC result;
    if (req->handler)
    {
        result = req->handler(req);
        if (result != AWRC_PROCESSING)
        {
            aworker_post_result(req, result);
        }
    }
    else
    {
        AWORKLOG("invalid handler, discard request");
        aworker_dismiss_request(req);
    }
}

static void aworker_dispatch_event(osiEvent_t *event)
{
    AWORKLOG("aworker_dispatch_event Unhandled event:%d", event->id);
}

static void aworker_dispatch_callback(void *param)
{
    AWORKER_RSP *rsp = (AWORKER_RSP *)param;
    if (rsp == NULL)
    {
        AWORKLOG("aworker_dispatch_callback invalid response");
        return;
    }
    if (rsp->callback)
    {
        rsp->callback(rsp->result, rsp->event, rsp->param);
    }
    free(rsp);
}

bool aworker_post_result(AWORKER_REQ *req, int result)
{
    bool ret = true;
    if (req == NULL)
    {
        AWORKLOG("aworker_post_callback invalid request");
        return false;
    }
    AWORKDBG("aworker_post_result result:%d", result);
    if (req->callback)
    {
        AWORKER_RSP *rsp = (AWORKER_RSP *)malloc(sizeof(AWORKER_RSP));
        if (rsp == NULL)
        {
            AWORKLOG("aworker_post_callback fail to malloc for response");
            ret = false;
        }
        else
        {
            rsp->callback = req->callback;
            rsp->event = req->event;
            rsp->param = req->param;
            rsp->result = result;
            osiThreadCallback(req->sender, aworker_dispatch_callback, (void *)rsp);
        }
    }
    aworker_dismiss_request(req);
    return ret;
}

static void aworker_dispatch_request(void *param)
{
    if (param == NULL)
    {
        AWORKLOG("aworker_dispatch_request invalid request");
        return;
    }
    aworker_handle_request((AWORKER_REQ *)param);
}

bool aworker_post_req_delay(AWORKER_REQ *req, uint32_t delay_ms, bool *result)
{
    bool ret = *result;
    if (!*result)
    {
        AWORKLOG("aworker_post_req_delay result value is not set to true, ignore post request");
        ret = false;
        goto out;
    }
    if (s_aworker_ctx.state != AWTS_STARTED)
    {
        AWORKLOG("aworker_post_req_delay async worker wasn't started, ignore this request");
        ret = false;
        goto out;
    }
    if (req != NULL && req->handler == NULL)
    {
        AWORKLOG("aworker_post_req_delay invalid request, must provide handler");
        ret = false;
        goto out;
    }
    if (delay_ms > 0)
    {
        if (s_aworker_ctx.postDelayTimerID != 0)
        {
            osiTimerStop(s_aworker_ctx.postDelayTimerID);
            osiTimerDelete(s_aworker_ctx.postDelayTimerID);
        }
        s_aworker_ctx.postDelayTimerID = osiTimerCreate(s_aworker_ctx.tid, aworker_dispatch_request, (void *)req);
        ret = osiTimerStart(s_aworker_ctx.postDelayTimerID, delay_ms);
    }
    else
    {
        ret = osiThreadCallback(s_aworker_ctx.tid, aworker_dispatch_request, (void *)req);
    }
out:
    if (!ret)
    {
        AWORKLOG("aworker_post_req_delay post request fail; delay_ms:%d", delay_ms);
        if (req != NULL)
        {
            aworker_dismiss_request(req);
        }
    }
    *result = ret;
    return ret;
}

bool aworker_post_handler(AWORKER_HANDLER handler, void *param, uint32_t delay_ms)
{
    AWORKER_REQ *req = (AWORKER_REQ *)malloc(sizeof(AWORKER_REQ));
    bool ret = true;
    if (req == NULL)
    {
        AWORKLOG("aworker_post_handler malloc for req fail");
        return false;
    }
    memset(req, 0, sizeof(AWORKER_REQ));
    req->handler = handler;
    req->param = param;
    return aworker_post_req_delay(req, delay_ms, &ret);
}

static void aworker_task_main(void *arg)
{
    osiEvent_t event;
    AWORKLOG("Enter aworker_task_main...");
    s_aworker_ctx.runnable = true;
    s_aworker_ctx.state = AWTS_STARTED;
    s_aworker_ctx.tid = osiThreadCurrent();
    while (true)
    {
        if (!s_aworker_ctx.runnable)
        {
            AWORKLOG("aworker_task_main exit message loop");
            break;
        }
        if (osiEventWait(s_aworker_ctx.tid, &event) == true)
        {
            AWORKLOG("aworker_dispatch_event...");
            aworker_dispatch_event(&event);
        }
        else
        {
            AWORKLOG("aworker osiEventWait... timeout");
        }
    }
    aworker_deinit();
}

void aworker_dump_bytes(uint8_t *bytes, uint16_t len)
{
    char *buffer;
    uint32_t bufflen = len * 3 + 1;
    uint16_t pos = 0;
    buffer = (char *)malloc(bufflen);
    if (buffer == NULL)
    {
        AWORKLOG("aworker_dump_bytes Fail to alloc buffer for len:%d", bufflen);
        return;
    }
    memset(buffer, 0, bufflen);
    for (pos = 0; pos < len; pos++)
    {
        sprintf(buffer + pos * 3, "%02x ", bytes[pos]);
    }
    AWORKLOG("aworker_dump_bytes: len:%d bytes:[%s]", len, buffer);
    free(buffer);
}

AWORKER_REQ *aworker_create_request(osiThread_t *sender, AWORKER_HANDLER handler,
                                    AWORKER_CALLBACK callback, uint32_t event, void *param, uint32_t bufflen)
{
    AWORKER_REQ_INT *req = NULL;
    if (bufflen == 0)
    {
        bufflen = AWORKER_DEF_BUFFER_LEN;
    }
    req = (AWORKER_REQ_INT *)malloc(sizeof(AWORKER_REQ_INT) + bufflen);
    if (req == NULL)
    {
        AWORKLOG("aworker_create_request Malloc REQ fail");
        return NULL;
    }
    memset(req, 0, sizeof(AWORKER_REQ_INT));
    req->req.sender = sender;
    req->req.handler = handler;
    req->req.callback = callback;
    req->req.event = event;
    req->req.param = param;
    req->buffer_len = bufflen;
    req->buffer = (uint8_t *)(req + 1);
    return (AWORKER_REQ *)req;
}

static inline bool aworker_buffer_is_enough(AWORKER_REQ_INT *req, uint32_t len)
{
    if (req->buffer_used < req->buffer_len && req->buffer_len - req->buffer_used >= len)
    {
        return true;
    }
    return false;
}

static inline void *aworker_align_address(void *address)
{
    uint32_t target = (uint32_t)address;
    target = (target >> 2 << 2) + ((target & 0x3) > 0 ? 4 : 0);
    AWORKDBG("aworker_assign_address target:%x address:%x", target, address);
    return (void *)target;
}

static inline void aworker_append_buffer(AWORKER_REQ_INT *req, void *buffer, uint32_t len)
{
    uint8_t *curr_ptr = req->buffer + req->buffer_used;
    uint8_t *next_start;
    memcpy(curr_ptr, buffer, len);
    next_start = aworker_align_address(curr_ptr + len);
    req->buffer_used += next_start - curr_ptr;
}
void aworker_param_putu32(AWORKER_REQ *req, uint32_t val, bool *result)
{
    AWORKER_REQ_INT *req_int = (AWORKER_REQ_INT *)req;
    AWORKER_PARAM_FORMAT format = {0};
    uint16_t len = sizeof(uint32_t);
    if (!*result)
    {
        return;
    }
    if (aworker_buffer_is_enough(req_int, len + sizeof(format)))
    {
        format.type = AWPT_UINT32;
        format.len = len;
        aworker_append_buffer(req_int, &format, sizeof(format));
        aworker_append_buffer(req_int, &val, format.len);
        req_int->param_count++;
        *result = true;
    }
    else
    {
        AWORKLOG("aworker_param_putu32 no enough buffer");
        *result = false;
    }
}

void aworker_param_putbytes(AWORKER_REQ *req, uint8_t *barray, uint16_t len, bool *result)
{
    AWORKER_REQ_INT *req_int = (AWORKER_REQ_INT *)req;
    AWORKER_PARAM_FORMAT format = {0};
    if (!*result)
    {
        return;
    }
    if (aworker_buffer_is_enough(req_int, len + sizeof(format)))
    {
        format.type = AWPT_BYTES;
        format.len = len;
        aworker_append_buffer(req_int, &format, sizeof(format));
        aworker_append_buffer(req_int, barray, format.len);
        req_int->param_count++;
        *result = true;
    }
    else
    {
        AWORKLOG("aworker_param_putbytes no enough buffer");
        *result = false;
    }
}

void aworker_param_putstr(AWORKER_REQ *req, char *str, bool *result)
{
    AWORKER_REQ_INT *req_int = (AWORKER_REQ_INT *)req;
    AWORKER_PARAM_FORMAT format = {0};
    uint16_t len = strlen((char *)str) + 1;
    if (!*result)
    {
        return;
    }
    if (aworker_buffer_is_enough(req_int, len + sizeof(format)))
    {
        format.type = AWPT_STRING;
        format.len = len;
        aworker_append_buffer(req_int, &format, sizeof(format));
        aworker_append_buffer(req_int, str, format.len);
        req_int->param_count++;
        *result = true;
    }
    else
    {
        AWORKLOG("aworker_param_putstr no enough buffer");
        *result = false;
    }
}

static uint8_t *aworker_get_param_ptr(AWORKER_REQ_INT *req, uint8_t index)
{
    uint8_t *ptr = req->buffer;
    uint8_t i = 0;
    AWORKER_PARAM_FORMAT *format;
    if (index > req->param_count - 1)
    {
        return NULL;
    }
    if (req->last_index <= index && req->last_ptr)
    {
        i = req->last_index;
        ptr = req->last_ptr;
    }
    AWORKDBG("aworker_get_param_ptr index:%d i:%d ptr:%x buffer:%x, bufflen:%d", index, i, ptr, req->buffer, req->buffer_len);
    while (i < index && ptr < req->buffer + req->buffer_len)
    {
        format = (AWORKER_PARAM_FORMAT *)ptr;
        ptr += sizeof(AWORKER_PARAM_FORMAT) + format->len;
        ptr = aworker_align_address(ptr);
        i++;
    }
    AWORKDBG("aworker_get_param_ptr after found i:%d ptr:%x", i, ptr);
    if (i == index && ptr < req->buffer + req->buffer_len)
    {
        req->last_index = index;
        req->last_ptr = ptr;
        return ptr;
    }
    return NULL;
}

uint32_t aworker_param_getu32(AWORKER_REQ *req, uint8_t index, bool *result)
{
    AWORKER_REQ_INT *req_int = (AWORKER_REQ_INT *)req;
    uint8_t *ptr;
    uint8_t type;
    uint16_t len;
    uint32_t val;
    AWORKER_PARAM_FORMAT *format;
    if (!*result)
    {
        return 0;
    }
    ptr = aworker_get_param_ptr(req_int, index);
    if (ptr == NULL)
    {
        AWORKLOG("aworker_param_getu32 can not found index:%s pointer", index);
        *result = false;
        return 0;
    }
    AWORKDBG("aworker_param_getu32 ptr:%x", ptr);
    format = (AWORKER_PARAM_FORMAT *)ptr;
    type = format->type;
    len = format->len;
    if (type != AWPT_UINT32)
    {
        AWORKLOG("aworker_param_getu32 unmatch type:%d", type);
        *result = false;
        return 0;
    }
    if (len != sizeof(uint32_t))
    {
        AWORKLOG("aworker_param_getu32 unmatch len:%d", len);
        *result = false;
        return 0;
    }
    val = *(uint32_t *)(ptr + sizeof(AWORKER_PARAM_FORMAT));
    return val;
}

char *aworker_param_getstr(AWORKER_REQ *req, uint8_t index, bool *result)
{
    AWORKER_REQ_INT *req_int = (AWORKER_REQ_INT *)req;
    uint8_t *ptr;
    uint8_t type;
    char *val;
    AWORKER_PARAM_FORMAT *format;
    if (!*result)
    {
        return 0;
    }
    ptr = aworker_get_param_ptr(req_int, index);
    if (ptr == NULL)
    {
        AWORKLOG("aworker_param_getstr can not found index:%s pointer", index);
        *result = false;
        return NULL;
    }
    AWORKDBG("aworker_param_getstr ptr:%x", ptr);
    format = (AWORKER_PARAM_FORMAT *)ptr;
    type = format->type;
    if (type != AWPT_STRING)
    {
        AWORKLOG("aworker_param_getstr unmatch type:%d", type);
        *result = false;
        return NULL;
    }
    val = (char *)(ptr + sizeof(AWORKER_PARAM_FORMAT));
    return val;
}

uint16_t aworker_param_getbytes(AWORKER_REQ *req, uint8_t index, uint8_t *buffer, uint16_t bufflen, bool *result)
{
    AWORKER_REQ_INT *req_int = (AWORKER_REQ_INT *)req;
    uint8_t *ptr;
    uint8_t type;
    uint16_t len;
    uint8_t *val;
    uint16_t outlen;
    AWORKER_PARAM_FORMAT *format;
    if (!*result)
    {
        return 0;
    }
    ptr = aworker_get_param_ptr(req_int, index);
    if (ptr == NULL)
    {
        AWORKLOG("aworker_param_getbytes can not found index:%s pointer", index);
        *result = false;
        return 0;
    }
    AWORKDBG("aworker_param_getbytes ptr:%x", ptr);
    format = (AWORKER_PARAM_FORMAT *)ptr;
    type = format->type;
    len = format->len;
    if (type != AWPT_BYTES)
    {
        AWORKLOG("aworker_param_getbytes unmatch type:%d", type);
        *result = false;
        return 0;
    }
    val = ptr + sizeof(AWORKER_PARAM_FORMAT);
    if (bufflen >= len)
    {
        outlen = len;
    }
    else
    {
        outlen = bufflen;
    }
    AWORKDBG("aworker_param_getbytes val:%x outlen:%d", val, outlen);
    memcpy(buffer, val, outlen);
    return outlen;
}

bool aworker_timer_callback(osiCallback_t tm_cb, uint32_t delay_ms, void *arg)
{
    if (s_aworker_ctx.timerID != 0)
    {
        osiTimerStop(s_aworker_ctx.timerID);
        osiTimerDelete(s_aworker_ctx.timerID);
    }
    s_aworker_ctx.timerID = osiTimerCreate(s_aworker_ctx.tid, tm_cb, arg);

    return osiTimerStart(s_aworker_ctx.timerID, delay_ms);
}

bool aworker_task_callback(osiCallback_t tcb, void *arg)
{
    return osiThreadCallback(s_aworker_ctx.tid, tcb, arg);
}
bool aworker_start()
{
    aworker_init();
    s_aworker_ctx.tid = osiThreadCreate("[Async Worker]",
                                        aworker_task_main,
                                        NULL,
                                        AWORKER_PRIORITY,
                                        AWORKER_STACK_SIZE,
                                        AWORKER_EVENT_QUEUE_SIZE);
    return true;
}

static AWORKER_RC aworker_stop_handler(AWORKER_REQ *req)
{
    AWORKLOG("aworker_stop_handler");
    return AWRC_SUCCESS;
}

/*    stop and delete task would cause system crash
void aworker_stop() {
    if (s_aworker_ctx.state != AWTS_STARTED) {
        return;
    }
    s_aworker_ctx.runnable = false;
    // send a stop message to wake up the task
    aworker_post_handler(aworker_stop_handler, NULL, 0);
    COS_Sleep(200);
    COS_StopTask(s_aworker_ctx.tid);
    COS_DeleteTask(s_aworker_ctx.tid);
}
*/
void aworker_test_param(void)
{
    uint32_t inu32 = 25;
    char *instr = "aworker test";
    uint8_t inbytes[4] = {0, 1, 2, 3};
    uint8_t outbytes[8] = {0};
    uint32_t outu32;
    char *outstr;
    uint16_t outbytelen;
    bool ret = true;
    AWORKER_REQ *req = aworker_create_request((osiThread_t *)1, aworker_stop_handler, NULL, 0, NULL, 0);
    if (req == NULL)
    {
        AWORKLOG("aworker_test_param fail");
        return;
    }
    AWORKLOG("before to put param inbytes:[%d %d %d %d], instr:%s inu32:%d", inbytes[0], inbytes[1], inbytes[2], inbytes[3],
             instr, inu32);
    aworker_param_putbytes(req, inbytes, sizeof(inbytes), &ret);
    aworker_param_putstr(req, instr, &ret);
    aworker_param_putu32(req, inu32, &ret);
    AWORKLOG("after put param and start to get; ret:%d", ret);
    outbytelen = aworker_param_getbytes(req, 0, outbytes, sizeof(outbytes), &ret);
    AWORKLOG("after aworker_param_getbytes ret:%d", ret);
    outstr = aworker_param_getstr(req, 1, &ret);
    AWORKLOG("after aworker_param_getstr ret:%d", ret);
    outu32 = aworker_param_getu32(req, 2, &ret);
    AWORKLOG("after aworker_param_getu32 ret:%d", ret);
    AWORKLOG("after get param ret:%d outbytelen:%d bytes:[%d %d %d %d %d %d] outstr:%s, outu32:%d", ret, outbytelen,
             outbytes[0], outbytes[1], outbytes[2], outbytes[3], outbytes[4], outbytes[5],
             outstr, outu32);
    aworker_dismiss_request(req);
    AWORKLOG("aworker_test_param Test end");
}

static void aworker_test_param_overflow()
{
    char *strparam = "1234567890abcdef";
    bool ret = true;
    AWORKLOG("aworker_test_param_overflow start");
    AWORKER_REQ *req = aworker_create_request((osiThread_t *)1, aworker_stop_handler, NULL, 0, NULL, strlen(strparam));
    AWORKLOG("Before put overflow string, ret should be true, ret:%d", ret);
    aworker_param_putstr(req, strparam, &ret);
    AWORKLOG("after put overflow string, ret should be false ret:%d", ret);
    aworker_dismiss_request(req);
    AWORKLOG("aworker_test_param_overflow end");
}

static AWORKER_RC aworker_test_handler(AWORKER_REQ *req)
{
    AWORKLOG("aworker_test_handler");
    return AWRC_SUCCESS;
}
static void aworker_test_callback(int result, uint32_t event, void *param)
{
    AWORKLOG("aworker_test_callback");
    AWORKLOG("aworker_test_handler_callback end");
}
void aworker_test_handler_callback(void)
{
    AWORKLOG("aworker_test_handler_callback start");
    bool ret = true;
    //aworker_start();
    AWORKER_REQ *req = aworker_create_request(s_aworker_ctx.tid, aworker_test_handler, aworker_test_callback, 0, NULL, 0);
    //COS_Sleep(200);
    aworker_post_req_delay(req, 0, &ret);
    //COS_Sleep(3000);
    //aworker_stop();
}
void aworker_test_main(void)
{
    aworker_test_param();
    aworker_test_param_overflow();
    aworker_test_handler_callback();
}
