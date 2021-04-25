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
#include "osi_log.h"
#include <stdlib.h>
#include "drv_ipc_at.h"
#include "drv_md_ipc.h"

struct at_dev
{
    int32_t chan_id;
    int32_t chan_stat;
    int32_t open_cnt;
    char *dev_name;
    struct smd_ch *ch;
    void *buf;
    int32_t count;

    //struct mutex lock;
    //wait_queue_head_t w_wait;
    //wait_queue_head_t r_wait;
};

static struct at_dev atdev_list[1] = {
    [0] = {
        .chan_stat = 0,
        .open_cnt = 0,
        .chan_id = SMD_CH_AT1,
        .dev_name = "ipc_at1",
    },
    // [1] = {
    //     .chan_stat = 0,
    //     .open_cnt = 0,
    //     .chan_id = SMD_CH_AT2,
    //     .dev_name = "ipc_at2",
    // },
};

#if 0
static int32_t ipc_at_open(struct at_dev *dev)
{
	struct at_dev *at = dev;
	at->open_cnt++;
	return 0;
}

static int32_t ipc_at_close(struct at_dev * dev)
{
	struct at_dev *at = dev;

	//mutex_lock(&at->lock);
	at->open_cnt--;
	//mutex_unlock(&at->lock);

	return 0;
}
#endif
static void dump(char *buf, int32_t len)
{
    int32_t i;

    OSI_LOGI(0, "buf:");
    for (i = 0; i < len; i++)
    {
        OSI_LOGI(0, "%02x", *buf++);
    }
}
static int32_t ipc_at_read(struct at_dev *dev)
{
    struct at_dev *atdev = dev;
    int32_t ret = 0, len = 0;

    OSI_LOGI(0, "ipc_at_read start\n");
    len = ipc_ch_read_avail(atdev->ch);
    if (len <= 0)
        return 0;

    if (len > dev->count)
        len = dev->count;

    ret = ipc_ch_read(atdev->ch, atdev->buf, len);
    if (ret < 0)
    {
        OSI_LOGI(0, "read data err\n");
    }
    OSI_LOGI(0, "atdev->ch %x", &atdev->ch);
    dump(atdev->buf, len);

    OSI_LOGI(0, "read: len = %d Bytes\n", ret);

    return ret;
}

static int32_t ipc_at_write(struct at_dev *dev)
{
    struct at_dev *atdev = dev;
    int32_t ret = 0, len = 0;

    OSI_LOGI(0, "ipc_at_write start\n");
    len = ipc_ch_write_avail(atdev->ch);
    if (len <= 0)
        return 0;

    if (len > atdev->count)
        len = atdev->count;

    ret = ipc_ch_write(atdev->ch, atdev->buf, len);
    if (ret < 0)
    {
        OSI_LOGI(0, "write data err\n");
    }
    OSI_LOGI(0, "write: len = %d Bytes\n", ret);

    return ret;
}

static void at_int_handler(void *priv, uint32_t event)
{
    OSI_LOGI(0, "at_int_handler start \n");
    struct at_dev *atdev = (struct at_dev *)priv;

    OSI_LOGI(0, "at_int_handler event %x \n", event);
    if (event & CH_READ_AVAIL)
    {
        ipc_at_read(atdev);
    }

    if (event & CH_WRITE_AVAIL)
    {
        ipc_at_write(atdev);
    }

    return;
}

int32_t ipc_at_init(void)
{
    struct at_dev *atdev = NULL;
    int32_t i = 0, ret = 0;
    int32_t ch_cnt = OSI_ARRAY_SIZE(atdev_list);
    char *buf = malloc(sizeof(uint8_t) * 512);

    for (i = 0; i < ch_cnt; i++)
    {
        atdev = &atdev_list[i];
        atdev->buf = buf;
        atdev->count = 5;

        ret = ipc_ch_open(atdev->chan_id, &atdev->ch, atdev,
                          at_int_handler);
        ipc_ch_set_event_mask(atdev->ch, CH_READ_AVAIL);
        if (ret < 0)
        {
            OSI_LOGI(0, "open channel failed.\n");
            atdev->chan_stat = 0;
            return -1;
        }
        else
        {
            OSI_LOGI(0, "open channel ok.\n");
            atdev->chan_stat = 1;
        }
    }

    return -1;
}
