// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

#ifndef INCLUDE_HANDLERS_H
#define INCLUDE_HANDLERS_H

#define HANDLER_NAME_MAX 30

#define HANDLER_MIN_PRIO -10
#define HANDLER_MAX_PRIO 20

#define HANDLER_REGISTERED 1
#define HANDLER_UNREGISTERED 0
#define HANDLER_BAD_CONF -1
#define HANDLER_NOT_FOUND -2

struct kfgx_cmd_struct;
typedef struct kfgx_token token_t;
typedef struct handler handler_t;
typedef struct kfgx_opt opt_t;
typedef int (*init_options_t)(handler_t *);
typedef int (*handler_action_t)(opt_t *opt);

/**
 * @brief Action callback signature executed when a handler predicate matches.
 *
 * @param[in,out] cmd Pointer to the command context structure.
 * @return 0 on success, or a negative error code on failure.
 */
typedef struct handler {
    handler_action_t action;
    init_options_t init_opt;
    const char *name;
    const int prio;
    token_t *ltokens; /* head (last add to the first) of Linked list of
                         associated options */
    struct handler *next;
} handler_t;

int kfgx_handler_init();

/**
 * @brief Sets the global default handler for CLI execution.
 *
 * Configures the fallback handler to be used when no explicit handler
 * matches the provided command line arguments or when no arguments are given.
 *
 * @param[in] a Pointer to the handler structure to set as default.
 *
 * @return 0 on success.
 * @return -1 if @p a is NULL or fails handler validation checks.
 *
 * @note The handler passed to this function should be properly initialized
 *       before being set as default.
 */
int kfgx_set_default_handler(handler_t *);

/** @brief Main CLI handling function. */
int kfgx_handle(struct kfgx_cmd_struct *);

/** @brief Validates a handler structure. */
int check_handler(const handler_t *);

/** @brief Resolves and assigns the matching handler action for a command
 * structure. */
int kfgx_get_handler(const struct kfgx_cmd_struct *);

/** @brief Registers a new task handler into the global priority list. */
int register_handler(handler_t *h);

/** @brief Unregisters and removes a specific task handler from the global list.
 */
int unregister_handler(handler_t *);

/** @brief Initializes the handler management subsystem structures. */
int kfgx_init_handlers(void);

/** @brief Cleans up and frees resources allocated by the handler subsystem. */
int kfgx_free_handlers(void);

#endif /*INCLUDE_HANDLERS_H*/