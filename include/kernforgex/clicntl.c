// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

/**
 * @file      clicntl.c
 * @author    jd
 * @brief
 * @version   0.1
 * @date      2026-08-30
 *
 * @details
 *
 * @copyright GNU General Public License v2.0
 */
#include "clicntl.h"
#include "debug.h"
#include "errors.h"
#include "prio_lists.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static struct lhead *lhandlers;
static size_t token_valid_values_nr = 7; // Initialisé aux 7 éléments par défaut

/**
 * @brief Array of supported command-line option rules.
 */
static kfgx_opt_rule token_valid_values[TOKEN_VALID_VALUES_LENGTH_MAX] = {
    {"--help", "-h", HANDLER_HELP},
    {"--all", "-a", HANDLER_ALL},
    {"--install", "-i", HANDLER_INST},
    {"--verbose", "-v", HANDLER_VERBOSE},
    {"--uninstall", "-u", HANDLER_UINST},
    {"--set-aliases", "-s", HANDLER_SETALS},
    {"--rm-aliases", "-r", HANDLER_RMALS},
};

int add_token_value(const kfgx_opt_rule r)
{
    if (handler_has_valid_rule(r)) {
        return -1;
    }
    if (token_valid_values_nr >= TOKEN_VALID_VALUES_LENGTH_MAX) {
        pr_info("can't add other options, limit reached");
        return -1;
    }
    token_valid_values[token_valid_values_nr] = r;
    token_valid_values_nr++;
    return 0;
}

static unsigned int kfgx_get_token_flags_impl(const token *t)
{
    unsigned int flags = 0;
    if (!t || !t->option)
        return flags;

    char *opt = t->option;

    // Usage / help check
    if (HAVE_LONG_OPTION(opt, "--help"))
        return HANDLER_HELP;

    /* Check direct long options using bound limit */
    for (size_t i = 1; i < token_valid_values_nr; i++) {
        const kfgx_opt_rule *r1 = &token_valid_values[i];
        if (r1->long_opt && HAVE_LONG_OPTION(opt, r1->long_opt)) {
            flags = r1->flag;
            return flags;
        }
    }

    if (opt && *opt == '-') {
        opt++;
        if (*opt == '-') {
            opt++;
            if (*opt == '\0') {
                return HANDLER_UNKWN;
            }
        }
    }

    /* Aggregate combined short options (e.g. -vu) */
    for (ssize_t i = (ssize_t)strlen(opt) - 1; i >= 0; i--) {
        int char_match = 0;

        for (size_t j = 1; j < token_valid_values_nr; j++) {
            const kfgx_opt_rule *r2 = &token_valid_values[j];
            if (!r2->short_opt)
                continue;

            char short_char =
                (r2->short_opt[0] == '-') ? r2->short_opt[1] : r2->short_opt[0];

            if (HAVE_SHORT_OPTION(opt[i], short_char)) {
                flags |= r2->flag;
                char_match = 1;
                break;
            }
        }

        if (!char_match) {
            return HANDLER_UNKWN;
        }
    }

    return flags;
}

int kfgx_get_token_flags(const token *t)
{
    pr_debug("%p", (void *)t);
    if (!t) {
        pr_error("token=%p", (void *)t);
        return -1;
    }

    if (t->free == TOKEN_DIRTY) {
        pr_error(IS_DIRTY_TOKEN);
        return -1;
    }

    return (int)kfgx_get_token_flags_impl(t); // bad but don't care
}

int has_valid_flag(const unsigned int flag)
{
    handler_t *h;
    if (!lhandlers)
        return 0;

    foreach_node(h, lhandlers->head)
    {
        unsigned int f = h->rule.flag;
        if ((f & flag) || f == flag)
            return -1;
    }
    return 0;
}

int handler_has_valid_rule(const kfgx_opt_rule r)
{
    handler_t *h;

    if (!lhandlers)
        return 0;

    // Test if flag is unique
    foreach_node(h, lhandlers->head)
    {
        unsigned int f = h->rule.flag;
        if ((f & r.flag) || f == r.flag) {
            pr_info("flag '%d' is already used or can cause conflict", r.flag);
            return -1;
        }
    }

    // Test long option uniqueness
    foreach_node(h, lhandlers->head)
    {
        const char *opt = h->rule.long_opt;
        if (opt && r.long_opt && HAVE_LONG_OPTION(opt, r.long_opt)) {
            pr_info("long option '%s' is already used", r.long_opt);
            return -1;
        }
    }

    // Test short option uniqueness
    foreach_node(h, lhandlers->head)
    {
        const char *opt = h->rule.short_opt;
        if (opt && r.short_opt && HAVE_LONG_OPTION(opt, r.short_opt)) {
            pr_info("short option '%s' is already used", r.short_opt);
            return -1;
        }
    }

    return 0;
}

static int kfgx_register_handler_impl(handler_t *h)
{
    if (prio_list_insert(lhandlers, h)) {
        pr_warn("failed to insert handler '%s'", h->name);
        return -1;
    }
    pr_info("registered handler '%s' with priority '%d'", h->name, h->prio);
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

    if (check_handler(h))
        return -1;

    return 0;
}

int kfgx_register_handler(handler_t *h)
{
    if (check_register_ctxt(h) != 0) {
        return -1;
    }
    return kfgx_register_handler_impl(h);
}

int kfgx_unregister_handler(handler_t *h)
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

int kfgx_get_handler(const struct kfgx_cmd_struct *cmd, handler_action_t *act)
{
    if (!cmd || !act || !lhandlers)
        return -1;

    handler_t *h;
    foreach_node(h, lhandlers->head)
    {
        if (h->rule.flag & cmd->flags) {
            *act = h->action;
            return 0;
        }
    }
    return -1;
}
