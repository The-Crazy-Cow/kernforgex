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

static const handler_action_t *default_handler;

/*
static int kfgx_get_handler_impl(struct kfgx_cmd_struct *cmd)
{
    token *t = cmd->ltokens;

    if (cmd->args_nr)
        return kfgx_default_handler(cmd);

    // analyze first token
    if (t->option) {
        cmd->flags = get_token_flags(t);

        //if( flags & HANDLER_FLAGS)
        //return; // handler(cmd)

    }

    return kfgx_default_handler(cmd);
}*/

int kfgx_set_default_handler(const handler_action_t *a)
{
    if (!a)
        return -1;
    default_handler = a;
    return 0;
}

static int kfgx_execute_handler_impl(struct kfgx_cmd_struct *cmd)
{
    handler_action_t act;

    if (kfgx_get_handler(cmd, &act)) {
        pr_info("failled to get appropriate handler");
        act = *default_handler;
    }
    return act(cmd);
}

int kfgx_execute_handler(struct kfgx_cmd_struct *cmd)
{
    if (!cmd) {
        pr_error("cmd=%p", (void *)cmd);
        return -1;
    }
    return kfgx_execute_handler_impl(cmd);
}
