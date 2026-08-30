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

static int kfgx_execute_handler_impl(struct kfgx_cmd_struct *cmd)
{
    return kfgx_get_handler(cmd);
}

int kfgx_execute_handler(struct kfgx_cmd_struct *cmd)
{
    if (!cmd) {
        pr_error("cmd=%p", (void *)cmd);
        return -1;
    }
    return kfgx_execute_handler_impl(cmd);
}
