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

#ifndef _OSI_ORDER_LIST_H_
#define _OSI_ORDER_LIST_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief opaque data struct for order list
 *
 * <em>order list</em> is a generic data struct for:
 * - Fixed memory size, allocated at creation.
 * - Use contiguous memory. So, the pointer of first can be casted into array.
 * - Record will be sorted at insert (similar to \p std::map).
 * - Record is unique (similar to \p std::map).
 * - User shall provide \p compar function (similar to \p bsearch) for sort.
 * - Provide API similar to \p std::lower_bound and \p std::upper_bound.
 * - Provide API  to find EQ, LT, GT.
 * - Optionally replace smallest record on full.
 *
 * Typical usage is to keep a sorted history, and it is needed to search from
 * the history.
 */
typedef struct osiOrderList osiOrderList_t;

/**
 * \brief create order list
 *
 * Signature of \p compar is the same as \p bsearch and \p qsort.
 *
 * \param size          size of each record
 * \param compar        record comparison function, return -1, 0, 1
 * \param max_count     maximum record count, can't be zero
 * \param replace       whether to replace smallest on full
 * \return
 *      - the created order list
 *      - NULL if out of memory, or invalid parameter
 */
osiOrderList_t *osiOrderListCreate(unsigned size, int (*compar)(const void *, const void *),
                                   unsigned max_count, bool replace);

/**
 * \brief delete the order list
 *
 * \param d             the order list instance
 */
void osiOrderListDelete(osiOrderList_t *d);

/**
 * \brief clear all records in order list
 *
 * \param d             the order list instance
 */
void osiOrderListClear(osiOrderList_t *d);

/**
 * \brief record count of order list
 *
 * \param d             the order list instance
 * \return
 *      - the record count
 */
unsigned osiOrderListCount(osiOrderList_t *d);

/**
 * \brief insert a record into order list
 *
 * When there exists a record which will match \p val, that is \p compar
 * will return 0, the existed record will be updated, and the existed
 * record will be returned.
 *
 * When record count reaches \p max_count, there are 2 strategies at
 * inserting new record:
 * - When \p replace is true, the smallest one will be dropped;
 * - When \p replace is false, return false;
 *
 * \param d             the order list instance
 * \return
 *      - the inserted record
 *      - NULL at full
 */
const void *osiOrderListInsert(osiOrderList_t *d, const void *val);

/**
 * \brief the fist record in order list
 *
 * \param d             the order list instance
 * \return
 *      - the first (smallest) record
 *      - NULL if the order list is empty
 */
const void *osiOrderListFirst(osiOrderList_t *d);

/**
 * \brief the last record in order list
 *
 * \param d             the order list instance
 * \return
 *      - the last (largest) record
 *      - NULL if the order list is empty
 */
const void *osiOrderListLast(osiOrderList_t *d);

/**
 * \brief find record in order list
 *
 * Find the record: <tt>record >= key</tt>. When all records are less than
 * \p val, the last record will be return. In this case,
 * <tt>record < key</tt>.
 *
 * Refer to <tt>std::lower_bound</tt>.
 *
 * \param d             the order list instance
 * \param val           record for find
 * \return
 *      - the lower bound record
 *      - NULL if the order list is empty
 */
const void *osiOrderListLowerBound(osiOrderList_t *d, const void *val);

/**
 * \brief find record in order list
 *
 * Find the record: <tt>record > key</tt>. When all records are less than
 * \p val, the last record will be return. In this case,
 * <tt>record <= key</tt>.
 *
 * Refer to <tt>std::upper_bound</tt>.
 *
 * \param d             the order list instance
 * \param key           key for find
 * \return
 *      - the upper bound record
 *      - NULL if the order list is empty
 */
const void *osiOrderListUpperBound(osiOrderList_t *d, const void *val);

/**
 * \brief find record in order list
 *
 * Find the record: <tt>record == key</tt>.
 *
 * \param d             the order list instance
 * \param key           key for find
 * \return
 *      - the match record
 *      - NULL if the order list is empty, or no record matches
 */
const void *osiOrderListFind(osiOrderList_t *d, const void *val);

/**
 * \brief find record in order list
 *
 * Find the record: <tt>record < key</tt>. When all records are greater or
 * equal than \p val, return NULL.
 *
 * \param d             the order list instance
 * \param key           key for find
 * \return
 *      - the upper bound record
 *      - NULL if the order list is empty, or no record matches
 */
const void *osiOrderListFindLT(osiOrderList_t *d, const void *val);

/**
 * \brief find record in order list
 *
 * Find the record: <tt>record > key</tt>. When all records are smaller or
 * equal than \p val, return NULL.
 *
 * \param d             the order list instance
 * \param key           key for find
 * \return
 *      - the upper bound record
 *      - NULL if the order list is empty, or no record matches
 */
const void *osiOrderListFindGT(osiOrderList_t *d, const void *val);

/**
 * \brief find record in order list
 *
 * Find the record: <tt>record <= key</tt>. When all records are greater or
 * equal than \p val, return NULL.
 *
 * \param d             the order list instance
 * \param key           key for find
 * \return
 *      - the upper bound record
 *      - NULL if the order list is empty, or no record matches
 */
const void *osiOrderListFindLE(osiOrderList_t *d, const void *val);

/**
 * \brief find record in order list
 *
 * Find the record: <tt>record >= key</tt>. When all records are smaller or
 * equal than \p val, return NULL.
 *
 * \param d             the order list instance
 * \param key           key for find
 * \return
 *      - the upper bound record
 *      - NULL if the order list is empty, or no record matches
 */
const void *osiOrderListFindGE(osiOrderList_t *d, const void *val);

/**
 * \brief helper function for unsigned integer compare
 *
 * Both \p a and \p b should be able to be casted into pointer of \p uint32_t,
 * and return the comparition of \p uint32_t.
 *
 * \param a             comparison parameter
 * \param b             comparison parameter
 * \return
 *      - 1 if <tt>*(const uint32_t *)a > *(const uint32_t *)b</tt>
 *      - 0 if <tt>*(const uint32_t *)a == *(const uint32_t *)b</tt>
 *      - -1 if <tt>*(const uint32_t *)a < *(const uint32_t *)b</tt>
 */
int osiUintCompare(const void *a, const void *b);

/**
 * \brief helper function for signed integer compare
 *
 * Both \p a and \p b should be able to be casted into pointer of \p int32_t,
 * and return the comparition of \p int32_t.
 *
 * \param a             comparison parameter
 * \param b             comparison parameter
 * \returnq
 *      - 1 if <tt>*(const int32_t *)a > *(const int32_t *)b</tt>
 *      - 0 if <tt>*(const int32_t *)a == *(const int32_t *)b</tt>
 *      - -1 if <tt>*(const int32_t *)a < *(const int32_t *)b</tt>
 */
int osiIntCompare(const void *a, const void *b);

#ifdef __cplusplus
}
#endif
#endif
