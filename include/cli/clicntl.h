// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

#ifndef INCLUDE_CLICNTL_H
#define INCLUDE_CLICNTL_H

#include "debug.h"
#include "handlers.h"
#include <stddef.h>

#define HAVE_SHORT_OPTION(option, a) ((a) == (option))
#define HAVE_LONG_OPTION(option, a) (!strcmp((option), (a)))

/* Token State Flags */
#define TOKEN_VALID 0x00
#define TOKEN_DIRTY 0x01

typedef struct kfgx_cmd_struct kfgx_cmd_t;
typedef struct handler handler_t;
typedef struct kfgx_opt opt_t;
typedef int (*init_options_t)(handler_t *h);
typedef struct kfgx_token token_t;

typedef struct kfgx_opt {
    char *l_opt; /* "--foo" long option */
    char *s_opt; /* "-f" short option */
    char *value;
    struct kfgx_opt *next;
} opt_t;

typedef struct kfgx_token {
    opt_t *opt;
    unsigned int free : 1;
    unsigned int match : 1;
    token_t *next;
} token_t;

struct kfgx_cmd_struct {
    handler_t *handler;
    const int args_nr;
    const char **args_set;
    int ret; /* Handler execution return code */
};

static inline token_t *kfgx_get_new_token(const opt_t *);

/**
 * @brief Add a new option token to a registered handler.
 *
 * Allocates a new token wrapping the provided option, initializes its flags,
 * and prepends it to the handler's token linked list.
 *
 * @param[in,out] h   Pointer to the target CLI handler.
 * @param[in]     opt Pointer to the option structure to register.
 *
 * @return 0 on success.
 * @return -1 if @p h is not registered, @p opt is NULL, or allocation fails.
 */
static inline int add_new_option(handler_t *h, const opt_t *opt)
{
    token_t *t;

    if (check_handler(h) != HANDLER_REGISTERED || !opt) {
        return -1;
    }

    t = kfgx_get_new_token(opt);
    if (!t) {
        pr_warn("failed to allocate new token");
        return -1;
    }

    t->match = 0;
    t->free = 1;

    if (t->opt) {
        t->opt->value = NULL;
    }

    t->next = h->ltokens;
    h->ltokens = t;

    return 0;
}

static inline int check_option(const opt_t *opt)
{
    if (!opt) {
        pr_warn("option pointer is NULL");
        return -1;
    }

    if ((!opt->l_opt || !*opt->l_opt) && (!opt->s_opt || !*opt->s_opt)) {
        pr_warn("invalid option: must specify at least long_opt or short_opt");
        return -1;
    }

    return 0;
}

/**
 * @brief Validate CLI arguments.
 *
 * @param[out] cmd       Parsed command structure.
 * @param[in]  args_nr   Expected number of arguments.
 * @param[in]  args_set  NULL-terminated argument array.
 *
 * @details In STRICT_MODE, also verifies that @p args_nr matches
 * the exact number of entries in @p args_set.
 *
 * @return 0 on success, -1 on error.
 */
static inline int check_cli_args(struct kfgx_cmd_struct *cmd)
{
    const char **args_set = cmd->args_set;
    int nr [[maybe_unused]];
    const char **set [[maybe_unused]];

    if (!cmd) {
        pr_warn("kfgx_cmd_struct *cmd=%p", (void *)cmd);
        return -1;
    }

    if (!args_set) {
        pr_warn("args_set=NULL, but args_nr=%d", cmd->args_nr);
        return -1;
    }

#ifdef STRICT_MODE
    nr = cmd->args_nr;
    set = args_set;

    while (*set++) {
        --nr;
    }

    if (nr != 0) {
        pr_fatal("args_set element count != args_nr, remaining=%d", nr);
        _exit(-1);
    }
#endif
    return 0;
}

#endif /* INCLUDE_CLICNTL_H */