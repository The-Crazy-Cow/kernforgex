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
#include "errors.h"
#include <sys/types.h>

/**
 * @brief Array of supported command-line option rules.
 *
 * @details Defines mapping between long/short option strings and their
 * corresponding handler flags. The array ends with a zeroed sentinel node
 * {0, 0, 0} to allow iteration without knowing the array size.
 */
static const kfgx_opt_rule token_valid_values[] = {
    {"--help", "-h", HANDLER_ALL},
    {"--all", "-a", HANDLER_ALL},
    {"--install", "-i", HANDLER_INST},
    {"--verbose", "-v", HANDLER_VERBOSE},
    {"--uninstall", "-u", HANDLER_UINST},
    {"--set-aliases", "-s", HANDLER_SETALS},
    {"--rm-aliases", "-r", HANDLER_RMALS},
    {0, 0, 0} // Sentinelle
};

static int get_token_flags_impl(token *t)
{
    int flags = 0;
    if (!t || !t->option)
        return flags;

    char *opt = t->option;
    const kfgx_opt_rule *r1 = token_valid_values + 1; // skip --help
    const kfgx_opt_rule *r2;

    // usage()
    if (HAVE_LONG_OPTION(opt, "--help")) // token_valid_values[0]
        goto end;

    /*
     * require to check user input coherence
     * like example we should never have :
     *       HANDLER_INST|HANDLER_UNIST
     *       HANDLER_SETALS|HANDLER_RMALS
     */
    for (; r1->long_opt != 0; r1++) {
        if (HAVE_LONG_OPTION(opt, r1->long_opt)) {
            flags = r1->has_value;
            goto end;
        }
    }

    if (opt && *opt == '-') {
        opt++;

        if (*opt == '-') {
            opt++;
            if (*opt == '\0') {
                flags = HANDLER_UNKWN;
                goto end;
            }
        }
    }

    // same warning like previous loop block
    // -> check user input coherence
    for (ssize_t i = (ssize_t)strlen(opt) - 1; i >= 0; i--) {
        int char_match = 0;
        r2 = token_valid_values + 1; // skip --help

        for (; r2->short_opt != 0; r2++) {
            char short_char =
                (r2->short_opt[0] == '-') ? r2->short_opt[1] : r2->short_opt[0];

            if (HAVE_SHORT_OPTION(opt[i], short_char)) {
                flags |= r2->has_value;
                char_match = 1;
                break;
            }
        }

        if (!char_match) {
            flags = HANDLER_UNKWN;
            goto end;
        }
    }

end:
    return flags;
}

/**
 * @brief Resolves and extracts command-line handler flags from a token.
 *
 * @details Traverses the option rule lookup table to match option strings.
 * It first attempts a direct full-string comparison for long/short options.
 * If unmatched, it strips leading '-' or '--' prefixes and iterates backward
 * through the option characters to aggregate flags for combined short options
 * (e.g., -iu), terminating early with HANDLER_UNKWN on invalid flags.
 *
 * @param[in] t Pointer to the token structure containing the option string.
 * @return Bitwise OR'd handler flags, or HANDLER_UNKWN if parsing fails.
 */
int get_token_flags(token *t)
{

    pr_debug("%p", t);
    if (!t) {
        pr_error("token=%p", (void *)t);
        return -1;
    }

    if (t->free == TOKEN_DIRTY) {
        pr_error(IS_DIRTY_TOKEN);
        return -1;
    }

    return get_token_flags_impl(t);
}