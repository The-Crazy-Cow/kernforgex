// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

#ifndef INCLUDE_PRIO_LISTS_H
#define INCLUDE_PRIO_LISTS_H

#include "clicntl.h"

#define foreach_node(n, _from) for (n = (_from); n != NULL; n = n->next)

/*Head control structure for managing the intrusive handler list
 * handler_t type is defined in clicntl.h
 */
struct lhead {
    handler_t *head;
    size_t size;
};

// lists operations
struct lhead *init_prio_list(void);
void free_prio_list(struct lhead *);
int prio_list_insert(struct lhead *self, handler_t *);
handler_t *prio_list_pop(struct lhead *);
handler_t *prio_list_del(struct lhead *, handler_t *);

#endif /*INCLUDE_PRIO_LISTS_H*/