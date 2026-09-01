// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

/**
 * @file      prio_lists.c
 * @author    jd
 * @brief
 * @version   0.1
 * @date      2026-08-31
 *
 * @details
 *
 * @copyright GNU General Public License v2.0
 */

#include "prio_lists.h"
#include <stdlib.h>
#include <sys/types.h>

handler_t *prio_list_del(struct lhead *self, handler_t *target)
{
    if (!self || !self->head || !target)
        return NULL;

    /* Case 1: Target is the head node */
    if (self->head == target) {
        self->head = target->next;
        target->next = NULL;
        self->size--;
        return target;
    }

    /* Case 2: Iterate to find target node */
    handler_t *curr = NULL;
    foreach_node(curr, self->head)
    {
        if (curr->next == target) {
            curr->next = target->next;
            target->next = NULL;
            self->size--;
            return target;
        }
    }

    return NULL;
}

int prio_list_insert(struct lhead *self, handler_t *h)
{
    if (!self || !h)
        return -1;

    h->next = NULL;

    /* Insert at head if list is empty or higher priority */
    if (!self->head || h->prio < self->head->prio) {
        h->next = self->head;
        self->head = h;
        self->size++;
        return 0;
    }

    /* Sorted insertion according to priority */
    handler_t *curr = self->head;
    while (curr->next && curr->next->prio <= h->prio) {
        curr = curr->next;
    }

    h->next = curr->next;
    curr->next = h;
    self->size++;
    return 0;
}

handler_t *prio_list_pop(struct lhead *self)
{
    if (!self || !self->head)
        return NULL;

    handler_t *extracted = self->head;
    self->head = self->head->next;
    extracted->next = NULL;
    self->size--;

    return extracted;
}

struct lhead *init_prio_list(void)
{
    struct lhead *l = malloc(sizeof(struct lhead));
    if (!l)
        return NULL;

    l->head = NULL;
    l->size = 0;
    return l;
}

void free_prio_list(struct lhead *l)
{
    if (!l)
        return;

    l->head = NULL;
    l->size = 0;
    free(l);
}