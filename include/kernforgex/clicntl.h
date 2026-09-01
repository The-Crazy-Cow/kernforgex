// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

#ifndef INCLUDE_KERNFORGE_CLICNTL_H
#define INCLUDE_KERNFORGE_CLICNTL_H

#include "debug.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define HANDLER_NAME_MAX 30
#define HANDLER_MIN_PRIO -10
#define HANDLER_MAX_PRIO 20

#define TOKEN_VALID_VALUES_LENGTH_MAX 100

/* Handler Bitmask Flags (Distinct Powers of 2) */
#define HANDLER_ALL 0x00
#define HANDLER_INST 0x01
#define HANDLER_UINST 0x02
#define HANDLER_SETALS 0x04
#define HANDLER_RMALS 0x08
#define HANDLER_VERBOSE 0x10
#define HANDLER_UNKWN 0x20
#define HANDLER_HELP 0x40

#define HAVE_SHORT_OPTION(option, a) ((a) == (option))
#define HAVE_LONG_OPTION(option, a) (!strcmp((option), (a)))

typedef struct {
    const char *long_opt;  /* "--foo" */
    const char *short_opt; /* "-f" */
    unsigned int flag;     /* Bitmask flag */
} kfgx_opt_rule;

/* Token State Flags */
#define TOKEN_VALID 0x00
#define TOKEN_DIRTY 0x01

typedef struct token {
    char *option;
    char *value;
    unsigned int free : 1; /* Token state allocation flag */
    struct token *next;
} token;

struct kfgx_cmd_struct {
    token *ltokens; /* Token linked list */
    const int args_nr;
    unsigned int flags;
    int ret; /* Handler execution return code */
};

/**
 * @brief Action callback signature executed when a handler predicate matches.
 *
 * @param[in,out] cmd Pointer to the command context structure.
 * @return 0 on success, or a negative error code on failure.
 */
typedef int (*handler_action_t)(struct kfgx_cmd_struct *cmd);

typedef struct handler {
    kfgx_opt_rule rule;
    handler_action_t action;
    char *name;
    int prio;
    struct handler *next;
} handler_t;

int handler_has_valid_rule(const kfgx_opt_rule r);

/**
 * @brief Validates the integrity and setup of a handler instance.
 *
 * @details Performs sanity checks on a task handler before registration or execution.
 * Verifies that the handler pointer is valid, has at least one associated option string
 * (long or short), possesses a non-NULL identifier name, includes a valid action callback,
 * contains no conflicting rules in the subsystem, and maintains a valid priority level.
 *
 * @param[in] h Pointer to the constant handler structure to validate.
 *
 * @return 0 if the handler is valid and safe to use, -1 if any validation fails.
 */
static inline int check_handler(const handler_t *h)
{
    if (!h) {
        pr_warn("handler pointer is NULL");
        return -1;
    }

    if (!(h->rule.long_opt || h->rule.short_opt || strlen(h->rule.long_opt) ||
          strlen(h->rule.short_opt))) {
        pr_warn("invalid handler rule: must specify at least long_opt or "
                "short_opt");
        return -1;
    }

    if (!h->name) {
        pr_error("handler must have a valid name string");
        return -1;
    }

    if (!h->action) {
        pr_warn("handler '%s' has a NULL action callback", h->name);
        return -1;
    }

    if (handler_has_valid_rule(h->rule) != 0) {
        return -1;
    }

    if (h->prio < HANDLER_MIN_PRIO || h->prio > HANDLER_MAX_PRIO) {
        pr_warn("handler '%s' has invalid priority %d", h->name, h->prio);
        return -1;
    }

    return 0;
}

/** @brief Retrieves status or property flags for a given token. */
int kfgx_get_token_flags(const token *t);

/** @brief Resolves and assigns the matching handler action for a command
 * structure. */
int kfgx_get_handler(const struct kfgx_cmd_struct *, handler_action_t *);

/** @brief Dispatches and runs the handler associated with the given command. */
int kfgx_execute_handler(struct kfgx_cmd_struct *cmd);

/** @brief Registers a new task handler into the global priority list. */
int kfgx_register_handler(handler_t *h);

/** @brief Unregisters and removes a specific task handler from the global list.
 */
int kfgx_unregister_handler(handler_t *h);

/** @brief Initializes the handler management subsystem structures. */
int kfgx_init_handlers(void);

/** @brief Cleans up and frees resources allocated by the handler subsystem. */
int kfgx_free_handlers(void);

int add_token_value(const kfgx_opt_rule r);
int kfgx_set_default_handler(const handler_action_t *a);

#endif /* INCLUDE_KERNFORGE_CLICNTL_H */