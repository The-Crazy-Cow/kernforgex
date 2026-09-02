// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

/**
 * @file      handle.c
 * @author    jd
 * @brief
 * @version   0.1
 * @date      2026-08-29
 *
 * @details
 *
 * @copyright GNU General Public License v2.0
 */

#include "debug.h"
#include "handlers.h"
#include "parser.h"
#include "prio_lists.h"

static handler_t *default_handler;
static struct lhead *lhandlers;

int check_handler(const handler_t *h)
{
    handler_t *h1;
    if (!h) {
        pr_warn("handler=%p", (void *)h);
        return HANDLER_BAD_CONF;
    }

    if (!h->name) {
        pr_error("handler must have a valid name string");
        return HANDLER_BAD_CONF;
    }

    if (!h->action) {
        pr_warn("handler '%s' has a NULL action callback", h->name);
        return HANDLER_BAD_CONF;
    }

    if (h->prio < HANDLER_MIN_PRIO || h->prio > HANDLER_MAX_PRIO) {
        pr_warn("handler '%s' has invalid priority %d", h->name, h->prio);
        return HANDLER_BAD_CONF;
    }

    foreach_node(h1, lhandlers->head)
    {
        if (h1 == h) {
            pr_info("handler '%s' is already registered", h->name);
            return HANDLER_REGISTERED;
        }
        if (h1->name && h->name && strcmp(h1->name, h->name) == 0) {
            pr_info("handler '%s' is already registered", h->name);
            return HANDLER_REGISTERED;
        }
    }

    return HANDLER_UNREGISTERED;
}

int kfgx_set_default_handler(handler_t *a)
{
    if (!a) {
        pr_warn("attempted to set NULL as default handler");
        return -1;
    }

    if (check_handler(a) != HANDLER_REGISTERED) {
        pr_warn(
            "handler '%s' must be registered before setting as default",
            a->name ? a->name : "unnamed");
        return -1;
    }

    default_handler = a;
    pr_info("default handler successfully set to '%s'", a->name);

    return 0;
}

static int kfgx_execute_handler(struct kfgx_cmd_struct *cmd)
{
    if (!cmd) {
        pr_error("cmd=%p", (void *)cmd);
        return -1;
    }
    if (!cmd->handler) {
        pr_error("cmd->handler=%p", (void *)cmd->handler);
        return -1;
    }
    cmd->ret = cmd->handler->action(cmd);
    return cmd->ret;
}

/**
 * @brief Processes and executes a CLI command payload.
 *
 * Handles the complete lifecycle of a CLI command execution: validates command
 * arguments, resolves the appropriate handler (falling back to @c
 * default_handler if unspecified or lookup fails), initializes handler-specific
 * options, tokenizes inputs, executes the payload, and performs comprehensive
 * memory cleanup.
 *
 * @param[in,out] cmd Pointer to the command context structure containing
 * argument vector and execution states.
 *
 * @return 0 on successful processing and execution.
 * @return -1 if @p cmd is NULL, validation fails, handler resolution fails
 * without a default, or token parsing fails.
 *
 * @note This function guarantees resource teardown by releasing the token list
 *       (@c ltokens) and tearing down registered handlers via @ref
 * kfgx_free_handlers before returning upon successful execution.
 *
 * @warning If argument parsing (@ref kfgx_cli_parser) fails, early exit occurs
 * without invoking handler execution or performing handler deallocation.
 */
int kfgx_handle(struct kfgx_cmd_struct *cmd)
{
    if (!cmd) {
        pr_warn("cmd=%p", (void *)cmd);
        return -1;
    }

    if (check_cli_args(cmd))
        return -1;

    if (cmd->args_nr == 0) {
        pr_debug("args_nr=0");
        goto deflt;
    }
    if (kfgx_get_handler(cmd) != 0) {
        pr_warn("Failed to get handler, using default handler");
        if (!default_handler) {
            pr_fatal("default handler not set");
            return -1;
        }
    deflt:
        cmd->handler = default_handler;
    }

    // Initialize handler options
    cmd->handler->init_opt(cmd->handler);

    // Tokenize CLI arguments
    if (kfgx_cli_parser(cmd)) {
        pr_warn("failled to parse command line arguments");
        return -1;
    }

    kfgx_execute_handler(cmd);

    // cleanup all
    kfgx_token_free(cmd->handler->ltokens);
    cmd->handler->ltokens = NULL; // ensure
    kfgx_free_handlers();

    return 0;
}

static int kfgx_register_handler_impl(handler_t *h)
{
    if (prio_list_insert(lhandlers, h)) {
        pr_warn("failed to insert handler '%s'", h->name);
        return -1;
    }
    pr_info("registered handler '%s' with priority=%d", h->name, h->prio);
    pr_debug("handlers list size=%zu", lhandlers->size);

    return 0;
}

static int kfgx_unregister_handler_impl(handler_t *h)
{
    if (!prio_list_del(lhandlers, h)) {
        pr_warn("failed to unregister handler '%s'", h->name);
        return -1;
    }
    pr_info("unregistered handler '%s'", h->name);
    pr_debug("handlers list size=%zu", lhandlers->size);

    return 0;
}

static int check_register_ctxt(const handler_t *h)
{
    if (!lhandlers) {
        pr_fatal(
            "handlers list not initialized, lhandlers=%p", (void *)lhandlers);
        exit(-1);
    }

    if (check_handler(h) != HANDLER_UNREGISTERED)
        return -1;

    return 0;
}

int register_handler(handler_t *h)
{
    if (check_register_ctxt(h) != 0) {
        return -1;
    }

    return kfgx_register_handler_impl(h);
}

int unregister_handler(handler_t *h)
{
    if (check_register_ctxt(h) != 0) {
        return -1;
    }
    return kfgx_unregister_handler_impl(h);
}

int kfgx_init_handlers(void)
{
    lhandlers = init_prio_list();
    if (lhandlers)
        return 0;

    pr_fatal("failed to initialize handlers list");
    exit(-1);
}

int kfgx_free_handlers(void)
{
    if (!lhandlers)
        return 0;

    /* Pop continuous elements until empty to safely avoid broken 'next'
     * pointers */
    handler_t *h;
    while ((h = prio_list_pop(lhandlers)) != NULL) {
        pr_info("unregistered handler '%s'", h->name);
    }

    free_prio_list(lhandlers);
    lhandlers = NULL;
    pr_debug("freed handlers list");
    return 0;
}

int kfgx_get_handler(const struct kfgx_cmd_struct *cmd)
{
    handler_t *h;
    if (!cmd || !cmd->handler)
        return -1;
    h = cmd->handler;

    if (check_register_ctxt(h)) {
        return -1;
    }
    foreach_node(h, lhandlers->head)
    {
        // foo@foo$ kfgx name_handler
        if (cmd->args_set && !strcmp(h->name, cmd->args_set[0])) {
            return 0;
        }
    }
    pr_warn(
        "no matching handler found for '%s'",
        cmd->args_set ? cmd->args_set[0] : "(null)");
    return -1;
}