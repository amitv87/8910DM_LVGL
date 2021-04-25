/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _SIMAGE_H_
#define _SIMAGE_H_

#include "osi_compiler.h"
#include "calclib/simage_types.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief whether simage header is valid
 *
 * It will perform sanity check of header, and the checksum of header.
 * \p max_size is the maximum permitted size, to avoid a huge wrong size
 * in header to cause trouble.
 *
 * \param header        simage header
 * \param max_size      maximum permitted size
 * \return
 *      - true if the header is valid
 *      - false if the header is invalid
 */
bool simageHeaderValid(const simageHeader_t *header, unsigned max_size);

/**
 * \brief whether simage payload is valid
 *
 * It will just verify the payload checksum. It must be called after
 * \p simageHeaderValid returns true.
 *
 * \param header        simage header
 * \return
 *      - true if the payload checksum is valid
 *      - false if the payload checksum is invalid
 */
bool simagePayloadValid(const simageHeader_t *header);

/**
 * \brief whether simage header and payload is valid
 *
 * It is the same to call \p simageHeaderValid and \p simagePayloadValid.
 *
 * \param header        simage header
 * \param max_size      maximum permitted size
 * \return
 *      - true if the header and payload are valid
 *      - false if the header or payload is invalid
 */
bool simageValid(const simageHeader_t *header, unsigned max_size);

/**
 * \brief whether certs inside are valid
 *
 * \param header        simage header
 * \return
 *      - true if all security checks are passed
 *      - false if there are security checks failed
 */
bool simageSignCheck(const simageHeader_t *header);

/**
 * \brief jump to the data entry address
 *
 * \param header        simage header
 * \param param         parameter for the entry
 */
OSI_NO_RETURN void simageJumpEntry(const simageHeader_t *header, unsigned param);

OSI_EXTERN_C_END
#endif
