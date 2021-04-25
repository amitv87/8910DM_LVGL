/*
 * RNDIS MSG parser
 *
 */

#define OSI_LOCAL_LOG_TAG OSI_MAKE_LOG_TAG('N', 'D', 'I', 'S')
#define OSI_LOCAL_LOG_LEVEL OSI_LOG_LEVEL_INFO

#include <osi_compiler.h>
#include <osi_api.h>
#include <osi_log.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/queue.h>

#include "rndis.h"

struct rndis_params
{
    int confignr;
    uint8_t used;
    enum rndis_state state;
    uint32_t medium;
    uint32_t speed;
    uint32_t media_state;
    uint16_t *filter;
    rndisData_t *ether;

    uint32_t vendorID;
    const char *vendorDescr;
    responseAvail_t response_avail;
    void *resp_param;

    rndisRespHead_t reserved_list;
    rndisRespHead_t ready_list;
};

#define rndis_debug 0

/* Driver Version */
static const uint32_t rndis_driver_version = cpu_to_le32(1);
static inline rndisResp_t *rndis_alloc_response(rndisParams_t *params, size_t length);

/* supported OIDs */
static const uint32_t oid_supported_list[] =
    {
        /* the general stuff */
        RNDIS_OID_GEN_SUPPORTED_LIST,
        RNDIS_OID_GEN_HARDWARE_STATUS,
        RNDIS_OID_GEN_MEDIA_SUPPORTED,
        RNDIS_OID_GEN_MEDIA_IN_USE,
        RNDIS_OID_GEN_MAXIMUM_FRAME_SIZE,
        RNDIS_OID_GEN_LINK_SPEED,
        RNDIS_OID_GEN_TRANSMIT_BLOCK_SIZE,
        RNDIS_OID_GEN_RECEIVE_BLOCK_SIZE,
        RNDIS_OID_GEN_VENDOR_ID,
        RNDIS_OID_GEN_VENDOR_DESCRIPTION,
        RNDIS_OID_GEN_VENDOR_DRIVER_VERSION,
        RNDIS_OID_GEN_CURRENT_PACKET_FILTER,
        RNDIS_OID_GEN_MAXIMUM_TOTAL_SIZE,
        RNDIS_OID_GEN_MEDIA_CONNECT_STATUS,
        RNDIS_OID_GEN_PHYSICAL_MEDIUM,

        /* the statistical stuff */
        RNDIS_OID_GEN_XMIT_OK,
        RNDIS_OID_GEN_RCV_OK,
        RNDIS_OID_GEN_XMIT_ERROR,
        RNDIS_OID_GEN_RCV_ERROR,
        RNDIS_OID_GEN_RCV_NO_BUFFER,
#ifdef RNDIS_OPTIONAL_STATS
        RNDIS_OID_GEN_DIRECTED_BYTES_XMIT,
        RNDIS_OID_GEN_DIRECTED_FRAMES_XMIT,
        RNDIS_OID_GEN_MULTICAST_BYTES_XMIT,
        RNDIS_OID_GEN_MULTICAST_FRAMES_XMIT,
        RNDIS_OID_GEN_BROADCAST_BYTES_XMIT,
        RNDIS_OID_GEN_BROADCAST_FRAMES_XMIT,
        RNDIS_OID_GEN_DIRECTED_BYTES_RCV,
        RNDIS_OID_GEN_DIRECTED_FRAMES_RCV,
        RNDIS_OID_GEN_MULTICAST_BYTES_RCV,
        RNDIS_OID_GEN_MULTICAST_FRAMES_RCV,
        RNDIS_OID_GEN_BROADCAST_BYTES_RCV,
        RNDIS_OID_GEN_BROADCAST_FRAMES_RCV,
        RNDIS_OID_GEN_RCV_CRC_ERROR,
        RNDIS_OID_GEN_TRANSMIT_QUEUE_LENGTH,
#endif /* RNDIS_OPTIONAL_STATS */

        /* mandatory 802.3 */
        /* the general stuff */
        RNDIS_OID_802_3_PERMANENT_ADDRESS,
        RNDIS_OID_802_3_CURRENT_ADDRESS,
        RNDIS_OID_802_3_MULTICAST_LIST,
        RNDIS_OID_802_3_MAC_OPTIONS,
        RNDIS_OID_802_3_MAXIMUM_LIST_SIZE,

        /* the statistical stuff */
        RNDIS_OID_802_3_RCV_ERROR_ALIGNMENT,
        RNDIS_OID_802_3_XMIT_ONE_COLLISION,
        RNDIS_OID_802_3_XMIT_MORE_COLLISIONS,
#ifdef RNDIS_OPTIONAL_STATS
        RNDIS_OID_802_3_XMIT_DEFERRED,
        RNDIS_OID_802_3_XMIT_MAX_COLLISIONS,
        RNDIS_OID_802_3_RCV_OVERRUN,
        RNDIS_OID_802_3_XMIT_UNDERRUN,
        RNDIS_OID_802_3_XMIT_HEARTBEAT_FAILURE,
        RNDIS_OID_802_3_XMIT_TIMES_CRS_LOST,
        RNDIS_OID_802_3_XMIT_LATE_COLLISIONS,
#endif /* RNDIS_OPTIONAL_STATS */

#ifdef RNDIS_PM
        /* PM and wakeup are "mandatory" for USB, but the RNDIS specs
	 * don't say what they mean ... and the NDIS specs are often
	 * confusing and/or ambiguous in this context.  (That is, more
	 * so than their specs for the other OIDs.)
	 *
	 * FIXME someone who knows what these should do, please
	 * implement them!
	 */

        /* power management */
        OID_PNP_CAPABILITIES,
        OID_PNP_QUERY_POWER,
        OID_PNP_SET_POWER,

#ifdef RNDIS_WAKEUP
        /* wake up host */
        OID_PNP_ENABLE_WAKE_UP,
        OID_PNP_ADD_WAKE_UP_PATTERN,
        OID_PNP_REMOVE_WAKE_UP_PATTERN,
#endif /* RNDIS_WAKEUP */
#endif /* RNDIS_PM */
};

/* NDIS Functions */
static int gen_ndis_query_resp(struct rndis_params *params, uint32_t OID, uint8_t *buf,
                               unsigned buf_len, rndisResp_t *r)
{
    int retval = -1;
    uint32_t length = 4; /* usually */
    uint32_t *outbuf;
    int i, count;

#ifdef VERBOSE_DEBUG
    uint32_t *tmp_buf = (uint32_t *)buf;
    if (buf_len && rndis_debug > 1)
    {
        OSI_LOGD(0, "query OID %08x value, len %d:\n", OID, buf_len);
        for (i = 0; i < buf_len; i += 16)
        {
            OSI_LOGD(0, "%03d: %08x %08x %08x %08x\n", i,
                     tmp_buf[i],
                     tmp_buf[i + 4],
                     tmp_buf[i + 8],
                     tmp_buf[i + 12]);
        }
    }
#endif

    /* response goes here, right after the header */
    drvEther_t *ether = usbEtherGetEther(params->ether->usbe);
    rndis_query_cmplt_type *resp = (rndis_query_cmplt_type *)r->buf;
    outbuf = (uint32_t *)&resp[1];
    resp->InformationBufferOffset = cpu_to_le32(16);
    const drvEthStats_t *status = &ether->stats;

    switch (OID)
    {

    /* general oids (table 4-1) */

    /* mandatory */
    case RNDIS_OID_GEN_SUPPORTED_LIST:
        OSI_LOGD(0, "RNDIS_OID_GEN_SUPPORTED_LIST\n");
        length = sizeof(oid_supported_list);
        count = length / sizeof(uint32_t);
        for (i = 0; i < count; i++)
            outbuf[i] = cpu_to_le32(oid_supported_list[i]);
        retval = 0;
        break;

    /* mandatory */
    case RNDIS_OID_GEN_HARDWARE_STATUS:
        OSI_LOGD(0, "RNDIS_OID_GEN_HARDWARE_STATUS\n");
        /* Bogus question!
		 * Hardware must be ready to receive high level protocols.
		 * BTW:
		 * reddite ergo quae sunt Caesaris Caesari
		 * et quae sunt Dei Deo!
		 */
        *outbuf = cpu_to_le32(0);
        retval = 0;
        break;

    /* mandatory */
    case RNDIS_OID_GEN_MEDIA_SUPPORTED:
        OSI_LOGD(0, "RNDIS_OID_GEN_MEDIA_SUPPORTED\n");
        *outbuf = cpu_to_le32(params->medium);
        retval = 0;
        break;

    /* mandatory */
    case RNDIS_OID_GEN_MEDIA_IN_USE:
        OSI_LOGD(0, "RNDIS_OID_GEN_MEDIA_IN_USE\n");
        /* one medium, one transport... (maybe you do it better) */
        *outbuf = cpu_to_le32(params->medium);
        retval = 0;
        break;

    /* mandatory */
    case RNDIS_OID_GEN_MAXIMUM_FRAME_SIZE:
        OSI_LOGD(0, "RNDIS_OID_GEN_MAXIMUM_FRAME_SIZE\n");
        *outbuf = 1500; /// MTU For ETHERNET
        retval = 0;
        break;

    /* mandatory */
    case RNDIS_OID_GEN_LINK_SPEED:
        if (rndis_debug > 1)
            OSI_LOGD(0, "RNDIS_OID_GEN_LINK_SPEED\n");
        if (params->media_state == RNDIS_MEDIA_STATE_DISCONNECTED)
            *outbuf = cpu_to_le32(0);
        else
            *outbuf = cpu_to_le32(params->speed);
        retval = 0;
        break;

    /* mandatory */
    case RNDIS_OID_GEN_TRANSMIT_BLOCK_SIZE:
        OSI_LOGD(0, "RNDIS_OID_GEN_TRANSMIT_BLOCK_SIZE\n");
        *outbuf = 1500; /// MTU For ETHERNET
        retval = 0;
        break;

    /* mandatory */
    case RNDIS_OID_GEN_RECEIVE_BLOCK_SIZE:
        OSI_LOGD(0, "RNDIS_OID_GEN_RECEIVE_BLOCK_SIZE\n");
        *outbuf = 1500; /// MTU For ETHERNET
        retval = 0;
        break;

    /* mandatory */
    case RNDIS_OID_GEN_VENDOR_ID:
        OSI_LOGD(0, "RNDIS_OID_GEN_VENDOR_ID\n");
        *outbuf = cpu_to_le32(params->vendorID);
        retval = 0;
        break;

    /* mandatory */
    case RNDIS_OID_GEN_VENDOR_DESCRIPTION:
        OSI_LOGD(0, "RNDIS_OID_GEN_VENDOR_DESCRIPTION\n");
        if (params->vendorDescr)
        {
            length = strlen(params->vendorDescr);
            memcpy(outbuf, params->vendorDescr, length);
        }
        else
        {
            outbuf[0] = 0;
        }
        retval = 0;
        break;

    case RNDIS_OID_GEN_VENDOR_DRIVER_VERSION:
        OSI_LOGD(0, "RNDIS_OID_GEN_VENDOR_DRIVER_VERSION\n");
        /* Created as LE */
        *outbuf = rndis_driver_version;
        retval = 0;
        break;

    /* mandatory */
    case RNDIS_OID_GEN_CURRENT_PACKET_FILTER:
        OSI_LOGD(0, "RNDIS_OID_GEN_CURRENT_PACKET_FILTER\n");
        *outbuf = cpu_to_le32(*params->filter);
        retval = 0;
        break;

    /* mandatory */
    case RNDIS_OID_GEN_MAXIMUM_TOTAL_SIZE:
        OSI_LOGD(0, "RNDIS_OID_GEN_MAXIMUM_TOTAL_SIZE\n");
        *outbuf = cpu_to_le32(RNDIS_MAX_TOTAL_SIZE);
        retval = 0;
        break;

    /* mandatory */
    case RNDIS_OID_GEN_MEDIA_CONNECT_STATUS:
        if (rndis_debug > 1)
            OSI_LOGD(0, "RNDIS_OID_GEN_MEDIA_CONNECT_STATUS\n");
        *outbuf = cpu_to_le32(params->media_state);
        retval = 0;
        break;

    case RNDIS_OID_GEN_PHYSICAL_MEDIUM:
        OSI_LOGD(0, "RNDIS_OID_GEN_PHYSICAL_MEDIUM\n");
        *outbuf = cpu_to_le32(0);
        retval = 0;
        break;

    /* The RNDIS specification is incomplete/wrong.   Some versions
	 * of MS-Windows expect OIDs that aren't specified there.  Other
	 * versions emit undefined RNDIS messages. DOCUMENT ALL THESE!
	 */
    case RNDIS_OID_GEN_MAC_OPTIONS: /* from WinME */
        OSI_LOGD(0, "RNDIS_OID_GEN_MAC_OPTIONS\n");
        *outbuf = cpu_to_le32(
            RNDIS_MAC_OPTION_RECEIVE_SERIALIZED | RNDIS_MAC_OPTION_FULL_DUPLEX);
        retval = 0;
        break;

    /* statistics OIDs (table 4-2) */

    /* mandatory */
    case RNDIS_OID_GEN_XMIT_OK:
        if (rndis_debug > 1)
            OSI_LOGD(0, "RNDIS_OID_GEN_XMIT_OK\n");
        if (ether)
        {
            *outbuf = cpu_to_le32(status->tx_packets - status->tx_errors - status->tx_dropped);
            retval = 0;
        }
        break;

    /* mandatory */
    case RNDIS_OID_GEN_RCV_OK:
        if (rndis_debug > 1)
            OSI_LOGD(0, "RNDIS_OID_GEN_RCV_OK\n");
        if (ether)
        {
            *outbuf = cpu_to_le32(status->rx_packets - status->rx_errors - status->rx_dropped);
            retval = 0;
        }
        break;

    /* mandatory */
    case RNDIS_OID_GEN_XMIT_ERROR:
        if (rndis_debug > 1)
            OSI_LOGD(0, "RNDIS_OID_GEN_XMIT_ERROR\n");
        if (ether)
        {
            *outbuf = cpu_to_le32(status->tx_errors);
            retval = 0;
        }
        break;

    /* mandatory */
    case RNDIS_OID_GEN_RCV_ERROR:
        if (rndis_debug > 1)
            OSI_LOGD(0, "RNDIS_OID_GEN_RCV_ERROR\n");
        if (ether)
        {
            *outbuf = cpu_to_le32(status->rx_errors);
            retval = 0;
        }
        break;

    /* mandatory */
    case RNDIS_OID_GEN_RCV_NO_BUFFER:
        OSI_LOGD(0, "RNDIS_OID_GEN_RCV_NO_BUFFER\n");
        if (ether)
        {
            *outbuf = cpu_to_le32(status->rx_dropped);
            retval = 0;
        }
        break;

    /* ieee802.3 OIDs (table 4-3) */

    /* mandatory */
    case RNDIS_OID_802_3_PERMANENT_ADDRESS:
        OSI_LOGD(0, "RNDIS_OID_802_3_PERMANENT_ADDRESS\n");
        if (params)
        {
            length = ETH_ALEN;
            memcpy(outbuf, params->ether->host_mac, length);
            retval = 0;
        }
        break;

    /* mandatory */
    case RNDIS_OID_802_3_CURRENT_ADDRESS:
        OSI_LOGD(0, "RNDIS_OID_802_3_CURRENT_ADDRESS\n");
        if (params)
        {
            length = ETH_ALEN;
            memcpy(outbuf, params->ether->host_mac, length);
            retval = 0;
        }
        break;

    /* mandatory */
    case RNDIS_OID_802_3_MULTICAST_LIST:
        OSI_LOGD(0, "RNDIS_OID_802_3_MULTICAST_LIST\n");
        /* Multicast base address only */
        *outbuf = cpu_to_le32(0xE0000000);
        retval = 0;
        break;

    /* mandatory */
    case RNDIS_OID_802_3_MAXIMUM_LIST_SIZE:
        OSI_LOGD(0, "RNDIS_OID_802_3_MAXIMUM_LIST_SIZE\n");
        /* Multicast base address only */
        *outbuf = cpu_to_le32(1);
        retval = 0;
        break;

    case RNDIS_OID_802_3_MAC_OPTIONS:
        OSI_LOGD(0, "RNDIS_OID_802_3_MAC_OPTIONS\n");
        *outbuf = cpu_to_le32(0);
        retval = 0;
        break;

    /* ieee802.3 statistics OIDs (table 4-4) */

    /* mandatory */
    case RNDIS_OID_802_3_RCV_ERROR_ALIGNMENT:
        OSI_LOGD(0, "RNDIS_OID_802_3_RCV_ERROR_ALIGNMENT\n");
        if (ether)
        {
            *outbuf = cpu_to_le32(status->rx_frame_errors);
            retval = 0;
        }
        break;

    /* mandatory */
    case RNDIS_OID_802_3_XMIT_ONE_COLLISION:
        OSI_LOGD(0, "RNDIS_OID_802_3_XMIT_ONE_COLLISION\n");
        *outbuf = cpu_to_le32(0);
        retval = 0;
        break;

    /* mandatory */
    case RNDIS_OID_802_3_XMIT_MORE_COLLISIONS:
        OSI_LOGD(0, "RNDIS_OID_802_3_XMIT_MORE_COLLISIONS\n");
        *outbuf = cpu_to_le32(0);
        retval = 0;
        break;

    default:
        OSI_LOGD(0, "query unknown OID 0x%08X\n", OID);
    }
    if (retval < 0)
        length = 0;

    resp->InformationBufferLength = cpu_to_le32(length);
    r->length = length + sizeof(*resp);
    resp->MessageLength = cpu_to_le32(r->length);
    return retval;
}

static int gen_ndis_set_resp(struct rndis_params *params, uint32_t OID,
                             uint8_t *buf, uint32_t buf_len)
{
    int retval = 0;
    uint32_t *tmp_buf = (uint32_t *)buf;

#ifdef VERBOSE_DEBUG
    if (buf_len && rndis_debug > 1)
    {
        OSI_LOGD(0, "set OID %08x value, len %d:\n", OID, buf_len);
        for (int i = 0; i < buf_len; i += 16)
        {
            OSI_LOGD(0, "%03d: %08x %08x %08x %08x\n", i,
                     tmp_buf[i],
                     tmp_buf[i + 4],
                     tmp_buf[i + 8],
                     tmp_buf[i + 12]);
        }
    }
#endif

    switch (OID)
    {
    case RNDIS_OID_GEN_CURRENT_PACKET_FILTER:

        /* these NDIS_PACKET_TYPE_* bitflags are shared with
		 * cdc_filter; it's not RNDIS-specific
		 * NDIS_PACKET_TYPE_x == USB_CDC_PACKET_TYPE_x for x in:
		 *	PROMISCUOUS, DIRECTED,
		 *	MULTICAST, ALL_MULTICAST, BROADCAST
		 */
        *params->filter = (uint16_t)tmp_buf[0];
        OSI_LOGI(0, "RNDIS_OID_GEN_CURRENT_PACKET_FILTER %08x", *params->filter);

        /* this call has a significant side effect:  it's
		 * what makes the packet flow start and stop, like
		 * activating the CDC Ethernet altsetting.
		 */
        if (*params->filter)
        {
            params->state = RNDIS_DATA_INITIALIZED;
            if (params->ether)
                usbEtherStart(params->ether->usbe);
            //netif_carrier_on(params->dev);
            //if (netif_running(params->dev))
            //	netif_wake_queue(params->dev);
        }
        else
        {
            params->state = RNDIS_INITIALIZED;
            //netif_carrier_off(params->dev);
            //netif_stop_queue(params->dev);
        }
        break;

    case RNDIS_OID_802_3_MULTICAST_LIST:
        /* I think we can ignore this */
        OSI_LOGI(0, "RNDIS_OID_802_3_MULTICAST_LIST\n");
        break;

    default:
        OSI_LOGW(0, "set unknown OID 0x%08X, size %d\n", OID, buf_len);
        retval = -EINVAL;
    }

    return retval;
}

static void rndis_notify_response_available(rndisParams_t *params, rndisResp_t *r)
{
    TAILQ_INSERT_TAIL(&params->ready_list, r, anchor);
    params->response_avail(params->resp_param);
}

/*
 * Response Functions
 */

static int rndis_init_response(struct rndis_params *params, rndis_init_msg_type *buf)
{
    OSI_LOGI(0, "RNDIS_MSG_INIT");
    if (!params->ether)
        return -ENOTSUP;

    rndisResp_t *r = rndis_alloc_response(params, sizeof(rndis_init_cmplt_type));
    if (!r)
        return -ENOMEM;

    rndis_init_cmplt_type *resp = (rndis_init_cmplt_type *)r->buf;
    resp->MessageType = cpu_to_le32(RNDIS_MSG_INIT_C);
    resp->MessageLength = cpu_to_le32(52);
    resp->RequestID = buf->RequestID; /* Still LE in msg buffer */
    resp->Status = cpu_to_le32(RNDIS_STATUS_SUCCESS);
    resp->MajorVersion = cpu_to_le32(RNDIS_MAJOR_VERSION);
    resp->MinorVersion = cpu_to_le32(RNDIS_MINOR_VERSION);
    resp->DeviceFlags = cpu_to_le32(RNDIS_DF_CONNECTIONLESS);
    resp->Medium = cpu_to_le32(RNDIS_MEDIUM_802_3);
    resp->MaxPacketsPerTransfer = cpu_to_le32(1);
    resp->MaxTransferSize = cpu_to_le32(
        1500 //params->dev->mtu
        + sizeof(struct ethhdr) + sizeof(struct rndis_packet_msg_type) + 22);
    resp->PacketAlignmentFactor = cpu_to_le32(0);
    resp->AFListOffset = cpu_to_le32(0);
    resp->AFListSize = cpu_to_le32(0);

    rndis_notify_response_available(params, r);
    return 0;
}

static int rndis_query_response(struct rndis_params *params, rndis_query_msg_type *buf)
{
    OSI_LOGI(0, "RNDIS_MSG_QUERY, %x/%x", buf->RequestID, buf->OID);
    if (!params->ether)
        return -ENOTSUP;

    /*
     * we need more memory:
     * gen_ndis_query_resp expects enough space for
     * rndis_query_cmplt_type followed by data.
     * oid_supported_list is the largest data reply
     */
    rndisResp_t *r = rndis_alloc_response(params, sizeof(oid_supported_list) + sizeof(rndis_query_cmplt_type));
    if (!r)
        return -ENOMEM;

    rndis_query_cmplt_type *resp = (rndis_query_cmplt_type *)r->buf;
    resp->MessageType = cpu_to_le32(RNDIS_MSG_QUERY_C);
    resp->RequestID = buf->RequestID; /* Still LE in msg buffer */

    if (gen_ndis_query_resp(params, buf->OID,
                            buf->InformationBufferOffset + 8 + (uint8_t *)buf,
                            buf->InformationBufferLength, r))
    {
        /* OID not supported */
        resp->Status = cpu_to_le32(RNDIS_STATUS_NOT_SUPPORTED);
        resp->MessageLength = cpu_to_le32(sizeof *resp);
        resp->InformationBufferLength = cpu_to_le32(0);
        resp->InformationBufferOffset = cpu_to_le32(0);
    }
    else
    {
        resp->Status = cpu_to_le32(RNDIS_STATUS_SUCCESS);
    }

    rndis_notify_response_available(params, r);
    return 0;
}

static int rndis_set_response(struct rndis_params *params, rndis_set_msg_type *buf)
{
    uint32_t BufLength = buf->InformationBufferLength;
    uint32_t BufOffset = buf->InformationBufferOffset;
    rndisResp_t *r = rndis_alloc_response(params, sizeof(rndis_set_cmplt_type));
    OSI_LOGI(0, "RNDIS_MSG_SET, %x/%x/%u/%u/%p", buf->RequestID, buf->OID,
             BufLength, BufOffset, r);
    if (!r)
        return -ENOMEM;

#ifdef VERBOSE_DEBUG
    OSI_LOGD(0, "Length: %d\n", BufLength);
    OSI_LOGD(0, "Offset: %d\n", BufOffset);
    OSI_LOGD(0, "InfoBuffer: ");

    for (i = 0; i < BufLength; i++)
    {
        OSI_LOGD(0, "%02x ", *(((uint8_t *)buf) + i + 8 + BufOffset));
    }

    OSI_LOGD(0, "\n");
#endif

    rndis_set_cmplt_type *resp = (rndis_set_cmplt_type *)r->buf;
    resp->MessageType = cpu_to_le32(RNDIS_MSG_SET_C);
    resp->MessageLength = cpu_to_le32(16);
    resp->RequestID = buf->RequestID; /* Still LE in msg buffer */
    if (gen_ndis_set_resp(params, buf->OID, ((uint8_t *)buf) + 8 + BufOffset, BufLength))
        resp->Status = cpu_to_le32(RNDIS_STATUS_NOT_SUPPORTED);
    else
        resp->Status = cpu_to_le32(RNDIS_STATUS_SUCCESS);

    rndis_notify_response_available(params, r);
    return 0;
}

static int rndis_reset_response(struct rndis_params *params, rndis_reset_msg_type *buf)
{
    OSI_LOGI(0, "RNDIS_MSG_RESET");
    rndisResp_t *r;
    while ((r = rndis_get_next_response(params)))
        rndis_free_response(params, r);

    r = rndis_alloc_response(params, sizeof(rndis_reset_cmplt_type));
    if (!r)
        return -ENOMEM;
    rndis_reset_cmplt_type *resp = (rndis_reset_cmplt_type *)r->buf;

    resp->MessageType = cpu_to_le32(RNDIS_MSG_RESET_C);
    resp->MessageLength = cpu_to_le32(16);
    resp->Status = cpu_to_le32(RNDIS_STATUS_SUCCESS);
    /* resent information */
    resp->AddressingReset = cpu_to_le32(1);

    rndis_notify_response_available(params, r);
    return 0;
}

static int rndis_keepalive_response(struct rndis_params *params,
                                    rndis_keepalive_msg_type *buf)
{
    OSI_LOGD(0, "RNDIS_MSG_KEEPALIVE");
    /* host "should" check only in RNDIS_DATA_INITIALIZED state */
    rndisResp_t *r = rndis_alloc_response(params, sizeof(rndis_keepalive_cmplt_type));
    if (!r)
        return -ENOMEM;

    rndis_keepalive_cmplt_type *resp = (rndis_keepalive_cmplt_type *)r->buf;
    resp->MessageType = cpu_to_le32(RNDIS_MSG_KEEPALIVE_C);
    resp->MessageLength = cpu_to_le32(16);
    resp->RequestID = buf->RequestID; /* Still LE in msg buffer */
    resp->Status = cpu_to_le32(RNDIS_STATUS_SUCCESS);

    rndis_notify_response_available(params, r);
    return 0;
}

/*
 * Device to Host Comunication
 */
static int rndis_indicate_status_msg(struct rndis_params *params, uint32_t status)
{
    rndis_indicate_status_msg_type *resp;
    if (params->state == RNDIS_UNINITIALIZED)
        return -1;

    rndisResp_t *r = rndis_alloc_response(params, sizeof(rndis_indicate_status_msg_type));
    if (!r)
        return -ENOMEM;
    resp = (rndis_indicate_status_msg_type *)r->buf;

    resp->MessageType = cpu_to_le32(RNDIS_MSG_INDICATE);
    resp->MessageLength = cpu_to_le32(20);
    resp->Status = cpu_to_le32(status);
    resp->StatusBufferLength = cpu_to_le32(0);
    resp->StatusBufferOffset = cpu_to_le32(0);

    rndis_notify_response_available(params, r);
    return 0;
}

int rndis_signal_connect(struct rndis_params *params)
{
    params->media_state = RNDIS_MEDIA_STATE_CONNECTED;
    return rndis_indicate_status_msg(params, RNDIS_STATUS_MEDIA_CONNECT);
}

int rndis_signal_disconnect(struct rndis_params *params)
{
    params->media_state = RNDIS_MEDIA_STATE_DISCONNECTED;
    return rndis_indicate_status_msg(params, RNDIS_STATUS_MEDIA_DISCONNECT);
}

void rndis_uninit(struct rndis_params *params)
{
    if (!params)
        return;
    params->state = RNDIS_UNINITIALIZED;

    rndisResp_t *resp;
    /* drain the response queue */
    while ((resp = rndis_get_next_response(params)))
        rndis_free_response(params, resp);
}

/*
 * Message Parser
 */
int rndis_msg_parser(struct rndis_params *params, uint8_t *buf)
{
    if (!params)
        return -ENODEV;

    if (!buf)
        return -ENOMEM;

    uint32_t *tmp = (uint32_t *)buf;
    uint32_t type = *tmp++;
    uint32_t length = *tmp++;

    /* NOTE: RNDIS is *EXTREMELY* chatty ... Windows constantly polls for
	 * rx/tx statistics and link status, in addition to KEEPALIVE traffic
	 * and normal HC level polling to see if there's any IN traffic.
	 */

    /* For USB: responses may take up to 10 seconds */
    switch (type)
    {
    case RNDIS_MSG_INIT:
        params->state = RNDIS_INITIALIZED;
        return rndis_init_response(params, (rndis_init_msg_type *)buf);

    case RNDIS_MSG_HALT:
        OSI_LOGI(0, "RNDIS_MSG_HALT\n");
        params->state = RNDIS_UNINITIALIZED;
        if (params->ether)
            usbEtherStop(params->ether->usbe);
        return 0;

    case RNDIS_MSG_QUERY:
        return rndis_query_response(params, (rndis_query_msg_type *)buf);

    case RNDIS_MSG_SET:
        return rndis_set_response(params, (rndis_set_msg_type *)buf);

    case RNDIS_MSG_RESET:
        return rndis_reset_response(params, (rndis_reset_msg_type *)buf);

    case RNDIS_MSG_KEEPALIVE:
        return rndis_keepalive_response(params, (rndis_keepalive_msg_type *)buf);

    default:
        /* At least Windows XP emits some undefined RNDIS messages.
		 * In one case those messages seemed to relate to the host
		 * suspending itself.
		 */
        OSI_LOGE(0, "unknown RNDIS message 0x%08X len %d", type, length);

        break;
    }

    return -1;
}

rndisParams_t *rndis_register(responseAvail_t resp_cb, void *p)
{
    if (resp_cb == NULL)
        return NULL;

    const unsigned req_cnt = 10;
    const unsigned req_buf_size = 256;
    const unsigned alloc_size = sizeof(struct rndis_params) + req_cnt * (req_buf_size + sizeof(rndisResp_t));
    struct rndis_params *params = (struct rndis_params *)malloc(alloc_size);
    if (params == NULL)
    {
        OSI_LOGE(0, "Fail to allocate rndis param");
        return NULL;
    }

    memset(params, 0, sizeof(*params));
    params->confignr = 0;
    params->used = 1;
    params->state = RNDIS_UNINITIALIZED;
    params->media_state = RNDIS_MEDIA_STATE_DISCONNECTED;
    params->resp_param = p;
    params->response_avail = resp_cb;

    TAILQ_INIT(&params->reserved_list);
    TAILQ_INIT(&params->ready_list);

    rndisResp_t *req = (rndisResp_t *)((uint8_t *)params + sizeof(rndisParams_t));
    uintptr_t exptr = (uintptr_t)req + sizeof(rndisResp_t) * req_cnt;
    for (unsigned i = 0; i < req_cnt; ++i)
    {
        req->buf = (uint8_t *)OSI_PTR_INCR_POST(exptr, req_buf_size);
        req->length = 0;
        TAILQ_INSERT_TAIL(&params->reserved_list, req, anchor);
        OSI_LOGI(0, "Rndis req 0x%x, buf 0x%x\n", req, req->buf);
        req++;
    }
    return params;
}

void rndis_deregister(struct rndis_params *params)
{
    OSI_LOGD(0, "rndis_deregister\n");
    if (!params)
        return;

    free(params);
}

int rndis_set_param_dev(struct rndis_params *params, rndisData_t *ether, uint16_t *cdc_filter)
{
    OSI_LOGD(0, "rndis_set_param_dev");
    if (ether == NULL || params == NULL || cdc_filter == NULL)
        return -EINVAL;

    uint32_t critical = osiEnterCritical();
    params->ether = ether;
    params->filter = cdc_filter;
    osiExitCritical(critical);
    return 0;
}

int rndis_set_param_vendor(struct rndis_params *params, uint32_t vendorID, const char *vendorDescr)
{
    OSI_LOGD(0, "rndis_set_param_vendor");
    uint32_t critical = osiEnterCritical();
    if (vendorDescr && params)
    {
        params->vendorID = vendorID;
        params->vendorDescr = vendorDescr;
        osiExitCritical(critical);
        return 0;
    }
    osiExitCritical(critical);
    return -1;
}

int rndis_set_param_medium(struct rndis_params *params, uint32_t medium, uint32_t speed)
{
    OSI_LOGD(0, "rndis_set_param_medium: %u %u\n", medium, speed);
    if (!params)
        return -1;

    uint32_t critical = osiEnterCritical();
    params->medium = medium;
    params->speed = speed;
    osiExitCritical(critical);

    return 0;
}

void rndis_free_response(struct rndis_params *params, rndisResp_t *resp)
{
    uint32_t critical = osiEnterCritical();
    TAILQ_INSERT_TAIL(&params->reserved_list, resp, anchor);
    osiExitCritical(critical);
}

rndisResp_t *rndis_get_next_response(struct rndis_params *params)
{
    uint32_t sc = osiEnterCritical();
    rndisResp_t *r = TAILQ_FIRST(&params->ready_list);
    if (r)
        TAILQ_REMOVE(&params->ready_list, r, anchor);
    osiExitCritical(sc);
    return r;
}

static inline rndisResp_t *rndis_alloc_response(rndisParams_t *params, size_t length)
{
    uint32_t sc = osiEnterCritical();
    rndisResp_t *r = TAILQ_FIRST(&params->reserved_list);
    if (r)
    {
        r->length = length;
        TAILQ_REMOVE(&params->reserved_list, r, anchor);
        OSI_LOGV(0, "add req 0x%x, buf 0x%x", r, r->buf);
    }
    osiExitCritical(sc);
    return r;
}
