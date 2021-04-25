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

#ifndef _USB__COMPOSITE_DEVICE_H_
#define _USB__COMPOSITE_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/queue.h>
#include <usb/usb_device.h>
#include <stdbool.h>

struct composite_gadget_s;
struct composite_func_s;

/**
 * @brief Composite gadget driver
 */
typedef struct composite_device_s cops_t;

typedef struct
{
    usbXfer_t *xfer;
    uint8_t *buffer;
    unsigned bufsize;
} copsCtrl_t;

typedef struct
{
    uint32_t (*fill_desc) (struct composite_func_s *f, void *buf, uint32_t size);
    int (*bind) (struct composite_func_s *f, cops_t *cops, udc_t *udc);
    void (*unbind) (struct composite_func_s *f);
    void (*destroy) (struct composite_func_s *f);
    int (*setup) (struct composite_func_s *f, const usb_device_request_t *ctrl);
    int (*set_alt) (struct composite_func_s *f, uint8_t intf, uint8_t alt);
    void (*disable) (struct composite_func_s *f);
} copsFuncOpt_t;

struct composite_func_s
{
    uint32_t name;
    copsFuncOpt_t ops;
    TAILQ_ENTRY(composite_func_s) anchor;
    // will be set on function add to composite device
    cops_t *cops;
    udc_t *controller;
};

/**
 * @brief Usb function adapt composite driver framework
 */
typedef struct composite_func_s copsFunc_t;

#define CHECK_CF_OPS(f, o) ((f) && ((f)->ops.o))

/**
 * @brief composite function fill its interface descriptors
 *
 * @param f     the function
 * @param buf   buffer to fill descriptors
 * @param size  size of the buffer
 * @return
 *      - (-1)      fail
 *      - otherwise the number of bytes actually filled
 */
static inline uint32_t cfFillDesc(copsFunc_t *f, void *buf, uint32_t size)
{
    if (size == 0 || buf == NULL)
        return 0;

    if (!CHECK_CF_OPS(f, fill_desc))
        return 0;

    return f->ops.fill_desc(f, buf, size);
}

/**
 * @brief composite function destroy
 *
 * @param f the composite function
 */
static inline void cfDestroy(copsFunc_t *f)
{
    if (CHECK_CF_OPS(f, destroy))
        return f->ops.destroy(f);
}

/**
 * @brief composite function process setup packet
 *
 * @param f     the function
 * @param ctrl  the setup packet
 * @return
 *      - (-1)  fail
 *      - 0     success
 */
static inline int cfSetup(copsFunc_t *f, const usb_device_request_t *ctrl)
{
    if (CHECK_CF_OPS(f, setup) && ctrl != NULL)
        return f->ops.setup(f, ctrl);
    return -1;
}

/**
 * @brief composite function bind driver
 *
 * @param f     the function
 * @param cops  the composite device
 * @param udc   the udc
 * @return
 *      - (-1)  fail
 *      - 0     success
 */
static inline int cfBind(copsFunc_t *f, cops_t *cops, udc_t *udc)
{
    if (CHECK_CF_OPS(f, bind) && cops && udc)
        return f->ops.bind(f, cops, udc);
    return -1;
}

/**
 * @brief composite function unbind driver
 *
 * @param f the function
 */
static inline void cfUnbind(copsFunc_t *f)
{
    if (CHECK_CF_OPS(f, unbind))
        f->ops.unbind(f);
}

/**
 * @brief composite function set alternate
 *
 * @param f     the function
 * @param intf  the interface number
 * @param alt   alternate setting number
 * @return
 *      - (-1)  fail
 *      - 0     success
 */
static inline int cfSetAlt(copsFunc_t *f, uint8_t intf, uint8_t alt)
{
    if (CHECK_CF_OPS(f, set_alt))
        return f->ops.set_alt(f, intf, alt);
    return -1;
}

/**
 * @breif composite function disable
 *
 * @param f     the function
 */
static inline void cfDisable(copsFunc_t *f)
{
    if (CHECK_CF_OPS(f, disable))
        f->ops.disable(f);
}

/**
 * @brief Create a composite gadget driver
 *
 * @return
 *      - NULL  fail to create the driver
 *      - other the composite driver
 */
cops_t *copsCreate();

/**
 * @brief Destroy a composite gadget driver
 *
 * @param c the composite driver
 */
void copsDestroy(cops_t *c);

/**
 * @brief Add an usb function to the composite driver
 *
 * @param c the composite driver
 * @param f the composite usb function
 * @return
 *      - true  success
 *      - false fail
 */
bool copsAddFunction(cops_t *c, copsFunc_t *f);

/**
 * @brief Add an usb function to the composite driver
 *
 * @param c     the composite driver
 * @param funcs the composite usb function list
 * @param count functions count
 * @return
 *      - true  success
 *      - false fail
 */
bool copsAddFunctions(cops_t *c, copsFunc_t **funcs, unsigned count);

/**
 * @brief Remove all usb function from the composite driver
 *
 * @param c the composite driver
 */
void copsRemoveAllFunction(cops_t *c);

/**
 * @brief From composite driver get the gadget driver instance
 *
 * @param c the composite driver
 * @return
 *      - NULL  fail
 *      - other the gadget driver instance
 */
udevDrv_t *copsGetGadgetDriver(cops_t *c);

/**
 * @brief Assign an interface number
 *
 * @param c the composite drvice
 * @param f the composite function
 * @return
 *      - (-1)      fail
 *      - [0:15]    the interface number
 */
int copsAssignInterface(cops_t *c, copsFunc_t *f);

/**
 * @brief Remove an interface
 *
 * @param c     the composite device
 * @param index the interface number
 */
void copsRemoveInterface(cops_t *c, uint8_t index);

/**
 * @brief Allocate a string id
 * @note Caller must keep the strings room, composite device only store
 *       the address.
 *
 * @param c     the composite device
 * @param ustr  the usb string (caller keep the memory)
 * @return      the string id, 0 means no string
 */
int copsAssignStringId(cops_t *c, usbString_t *ustr);

/**
 * @brief Remove a string, giveback the id
 *
 * @param c     the composite device
 * @prarm ustr  the usb string (caller keep the memory)
 * @return
 *      - 0     always return 0
 */
int copsRemoveString(cops_t *c, usbString_t *ustr);

/**
 * @brief Set vendor id for this composite device
 *
 * @param c     the composite device
 * @param vid   vid
 */
void copsSetVendorId(cops_t *c, uint16_t vid);

/**
 * @brief Set product id for this composite device
 *
 * @param c     the composite device
 * @param pid   pid
 */
void copsSetProductId(cops_t *c, uint16_t pid);

/**
 * @brief get ctrl buffer/xfer
 *
 * @param c     the composite device
 * @return
 *      - NonNull for cops ctrl data
 */
copsCtrl_t *copsGetCtrl(cops_t *c);

#ifdef __cplusplus
}
#endif

#endif
