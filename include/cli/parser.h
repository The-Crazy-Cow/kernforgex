// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

#ifndef KERNFORGE_PARSER_H
#define KERNFORGE_PARSER_H

#include "clicntl.h"
#include <stdlib.h>
#include <string.h>

#define MATCH_L_OPT(opt, arg) (!strcmp((opt), (arg)))
#define MATCH_S_OPT(opt, arg) (!strcmp((opt), (arg)))

int kfgx_cli_parser(struct kfgx_cmd_struct *);
int kfgx_cli_tokenizer(struct kfgx_cmd_struct *);
void kfgx_cli_token_free(token_t *); // tokens list

[[maybe_unused]]
static inline int set_token_dirty(token_t *t)
{
    if (!t)
        return -1;
    t->free = TOKEN_DIRTY;
    return 0;
}

static inline int set_token_valid(token_t *t)
{
    if (!t)
        return -1;
    t->free = TOKEN_VALID;
    return 0;
}

/**
 * @brief Free a CLI token list and its associated options.
 *
 * @param[in,out] t Head of the token linked list to release.
 *
 * @details Traverses the singly linked list starting at @p t. If the @c free
 *          flag of a token node is set, all allocated option fields and th
 *          option structure itself are released. Finally, frees each token
 * node.
 *
 * @note Safe to call with a NULL pointer.
 */
static inline void kfgx_token_free(token_t *t)
{
    token_t *next;

    if (!t) {
        pr_debug("token=%p: RAF & UAF risks", (void *)t);
        return;
    }

    while (t) {
        next = t->next;
        pr_debug("%s", t->opt->l_opt);
        if (t->free && t->opt) {
            free(t->opt->l_opt);
            free(t->opt->s_opt);
            free(t->opt->value);
            free(t->opt);
        }

        free(t);
        t = next;
    }
}

static inline token_t *kfgx_get_new_token(const opt_t *opt)
{
    token_t *t;

    if (check_option(opt)) {
        pr_warn("invalid option");
        return NULL;
    }

    t = calloc(1, sizeof(token_t));
    if (!t) {
        return NULL;
    }

    t->opt = malloc(sizeof(opt_t));
    if (!t->opt) {
        free(t);
        return NULL;
    }

    t->opt->l_opt = opt->l_opt ? strdup(opt->l_opt) : NULL;
    t->opt->s_opt = opt->s_opt ? strdup(opt->s_opt) : NULL;
    t->opt->value = opt->value ? strdup(opt->value) : NULL;

    t->next = NULL;
    t->free = 1;
    set_token_valid(t);

    return t;
}

#endif /* KERNFORGE_PARSER_H */