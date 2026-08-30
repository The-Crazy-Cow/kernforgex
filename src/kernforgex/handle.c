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
