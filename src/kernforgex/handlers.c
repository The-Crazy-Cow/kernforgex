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

#include "clicntl.h"
#include "debug.h"
#include "parser.h"

/**
 * @brief Default fallback handler executed when no specific handler matches the
 * command flags.
 *
 * @details Serves as the primary error handler or default action processor for
 * command line inputs. It evaluates the aggregated option flags (such as
 * HANDLER_UNKWN, help flags, or absence of flags) to display contextual usage
 * instructions, issue warnings, or trigger default operations on the command
 * structure.
 *
 * @param[in,out] cmd Pointer to the command structure holding parsed tokens and
 * runtime context.
 *
 * @return 0 on success, or a negative error code
 */
static int kfgx_default_handler(struct kfgx_cmd_struct *cmd)
{

#ifdef DEBUG
    // check tokens are really passed
    token *t = cmd->ltokens;
    while (t) {
        if (t->option)
            printf("option: %s", t->option);

        if (t->value)
            printf(" | value: %s", t->value);

        printf("\n");
        t = t->next;
    }
    printf("\nflags := %d\n", cmd->flags);
    //--------------------------------------
#endif

    return 0;
}

static int kfgx_get_handler_impl(struct kfgx_cmd_struct *cmd)
{
    token *t = cmd->ltokens;

    if (cmd->args_nr)
        return kfgx_default_handler(cmd);

    // analyze first token
    if (t->option) {
        cmd->flags = get_token_flags(t);

        /*
         *if( flags & HANDLER_FLAGS)
         *    return; // handler(cmd)
         */
    }

    return kfgx_default_handler(cmd);
}

/**
 * @brief Evaluates command tokens and dispatches the execution flow to the
 * appropriate handler.
 *
 * @details Inspects the initial token from the command's token list
 * (`cmd->ltokens`). If the token contains a valid option string, its
 * corresponding bitwise handler flags are extracted via get_token_flags().
 * Depending on the resolved flags, the command can be routed to a specific
 * subsystem handler or fall back to the default handler mechanism.
 *
 * @param[in,out] cmd Pointer to the command structure containing parsed tokens
 * and context.
 *
 * @return Status code returned by the executed handler, 0 or  a negative error
 * code.
 */
int kfgx_get_handler(struct kfgx_cmd_struct *cmd)
{
    if (!cmd) {
        pr_warn("cmd=%p", (void *)cmd);
        return -1;
    }
    return kfgx_get_handler_impl(cmd);
}
