// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

/**
 * @file      handlers.c
 * @author    jd
 * @brief
 * @version   0.1
 * @date      2026-09-01
 *
 * @details
 *
 * @copyright GNU General Public License v2.0
 */

#include "clicntl.h"
#include "debug.h"
#include "parser.h"
#include "prio_lists.h"

int kfgx_default_handler_action(struct kfgx_cmd_struct *cmd)
{
    (void)cmd;
    pr_debug("hello world from default handler");

    token *t;
    foreach_node(t,cmd->ltokens){
        pr_info("valaue:=%s:opt:=%s",t->value,t->option);
    }
    return 0;
}

// default handler
static handler_t kfgx_handler = {
    .rule =
        {
            .long_opt = "",
            .short_opt = "",
            .flag = 0X0,
        },
    .action = kfgx_default_handler_action,
    .name = "default",
    .prio = -1,
};

// default handler
int kfgx_handler_init()
{
    if (kfgx_register_handler(&kfgx_handler)) {
        pr_warn("default handler init faillled");
        return -1;
    }
    if (kfgx_set_default_handler(&kfgx_handler.action)) {
        pr_warn("set default handler failled");
        return -1;
    }
    return 0;
}
