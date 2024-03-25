/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_LIST_SORT_H
#define _LINUX_LIST_SORT_H

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#include <stdbool.h>
#include "list.h"
#include "stdint.h"

struct list_head;

typedef int
    __attribute__((nonnull(1, 2))) (*list_cmp_func_t)(const struct list_head *,
                                                      const struct list_head *);

__attribute__((nonnull(1, 2))) void list_sort(struct list_head *head,
                                              list_cmp_func_t cmp,
                                              bool descend);
#endif

/**
 * q_sort2() - Same as q_sort(), but with using linux list_sort
 * @head: header of queue
 * @descend: whether or not to sort in descending order
 *
 * No effect if queue is NULL or empty. If there has only one element, do
 * nothing.
 *
 * Implement at queue.c
 */
void q_sort2(struct list_head *head, bool descend);