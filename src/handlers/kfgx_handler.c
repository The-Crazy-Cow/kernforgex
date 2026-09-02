// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

/**
 * @file      kfgx_handler.c
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
#include "handlers.h"
#include "parser.h"
#include "prio_lists.h"

static void kfgx_default_handler_usage(void)
{
    pr_info("default handler usage");
    pr_info("Usage: kfgx [options]");
    pr_info("Options:");
    pr_info("  --help, -h     Show this help message");
    pr_info("  --version, -v  Show version information");
    pr_info("  --verbose, -V  Enable verbose output");
}

static int init_options_default(handler_t *h)
{
    add_new_option(
        h,
        &(opt_t){
            .l_opt = "--help", .s_opt = "-h", .value = NULL, .next = NULL});
    add_new_option(
        h,
        &(opt_t){
            .l_opt = "--version", .s_opt = "-v", .value = NULL, .next = NULL});
    add_new_option(
        h,
        &(opt_t){
            .l_opt = "--verbose", .s_opt = "-t", .value = NULL, .next = NULL});
    return 0;
}

static int kfgx_default_handler_action(opt_t *options)
{
    if (!options) {
        pr_debug("options=%p",(void *)options);
    }

    kfgx_default_handler_usage();
    return 0;
}

// default handler
static handler_t kfgx_handler = {
    .action = kfgx_default_handler_action,
    .init_opt = init_options_default,
    .name = "default",
    .prio = -1,
    .ltokens = NULL,
    .next = NULL,
};

// default handler
int kfgx_handler_init()
{
    if (register_handler(&kfgx_handler)) {
        pr_warn("failed to register default handler");
        return -1;
    }

    if (kfgx_set_default_handler(&kfgx_handler)) {
        pr_warn("failed to set default handler");
        return -1;
    }

    return 0;
}