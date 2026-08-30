// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

/**
 * @file      default_handler.c
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
int kfgx_default_handler(struct kfgx_cmd_struct *cmd)
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
