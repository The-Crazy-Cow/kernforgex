// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

/**
 * @file      parser.c
 * @author    jd
 * @brief     Parse and tokenize the user prompt.
 * @version   0.1
 * @date      2026-08-29
 *
 * @details
 *
 * @copyright GNU General Public License v2.0
 */

#include "parser.h"
#include "debug.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

[[maybe_unused]]
inline int set_token_dirty(token *t)
{
    if (!t)
        return -1;
    t->free = TOKEN_DIRTY;
    return 0;
}

static inline int set_token_valid(token *t)
{
    if (!t)
        return -1;
    t->free = TOKEN_VALID;
    return 0;
}

/**
 * @brief Validate CLI arguments.
 *
 * @param[out] cmd       Parsed command structure.
 * @param[in]  args_nr   Expected number of arguments.
 * @param[in]  args_set  NULL-terminated argument array.
 *
 * @details In STRICT_MODE, also verifies that @p args_nr matches
 * the exact number of entries in @p args_set.
 *
 * @return 0 on success, -1 on error.
 */
static int check_cli_args(struct kfgx_cmd_struct *cmd, const char **args_set)
{
    int nr [[maybe_unused]];
    const char **set [[maybe_unused]];

    if (!cmd) {
        pr_warn("kfgx_cmd_struct *cmd=%p", (void *)cmd);
        return -1;
    }

    if (!args_set) {
        pr_warn("args_set=NULL, but args_nr=%d", cmd->args_nr);
        return -1;
    }

#ifdef STRICT_MODE
    nr = cmd->args_nr;
    set = args_set;

    while (*set++) {
        --nr;
    }

    if (nr != 0) {
        pr_fatal("args_set element count != args_nr, remaining=%d", nr);
        _exit(-1);
    }
#endif

    return 0;
}

/**
 * @brief Free a CLI token list.
 *
 * @param[in,out] tokens  Head of the token list to release.
 *
 * @details Traverses the linked list and frees token fields if the
 * free flag is enabled.
 */
void kfgx_cli_token_free(token *tokens)
{
    token *next;

    if (!tokens) {
        pr_debug("tokens list empty: RAF & UAF risks"); // Reused after free /
                                                        // Use-after-free
    }

    while (tokens) {
        next = tokens->next;

        if (tokens->free) {
            free(tokens->option);
            free(tokens->value);
        }

        free(tokens);
        tokens = next;
    }
}

/**
 * @brief Allocate and initialize a CLI token.
 *
 * @param[in] option  Option string (e.g., "-foo").
 * @param[in] value   Associated value string, or NULL if flags-only.
 *
 * @return Pointer to allocated token, or NULL on failure.
 */
static token *kfgx_cli_get_new_token(const char *option, const char *value)
{
    token *t;

    t = calloc(1, sizeof(*t));
    if (!t)
        return NULL;

    t->option = strdup(option);
    if (!t->option)
        goto error;

    if (value) {
        t->value = strdup(value);
        if (!t->value)
            goto error;
    }

    set_token_valid(t);
    return t;

error:
    free(t->option);
    free(t->value);
    free(t);

    return NULL;
}

static int
kfgx_cli_tokenizer_impl(struct kfgx_cmd_struct *cmd, const char **args_set)
{
    const char **set;
    const char *tok;
    const char *equal;

    token *curr = NULL;
    token *new_token;

    // Locate the tail if a token list already exists
    if (cmd->ltokens) {
        curr = cmd->ltokens;
        while (curr->next)
            curr = curr->next;
    }

    set = args_set;

    while (*set) {
        tok = *set;

        equal = strchr(tok, '=');

        if (equal) {
            /*
             * Key-value format "-foo=bar"
             * option : "-foo"
             * value  : "bar"
             */
            size_t opt_len = (size_t)(equal - tok);
            char *opt_tmp = malloc(opt_len + 1);
            if (!opt_tmp)
                return -1;

            strncpy(opt_tmp, tok, opt_len);
            opt_tmp[opt_len] = '\0';

            new_token = kfgx_cli_get_new_token(opt_tmp, equal + 1);
            free(opt_tmp);
        } else {
            /*
             * Flag format "-foo" | "foo"
             * option : "-foo" | foo
             * value  : NULL
             */
            new_token = kfgx_cli_get_new_token(tok, NULL);
        }

        if (!new_token) {
            pr_warn(
                "new_token=%p failed on token allocation", (void *)new_token);
            return -1;
        }

        // Append to list
        if (!cmd->ltokens)
            cmd->ltokens = new_token;
        else
            curr->next = new_token;

        curr = new_token;

        ++set;
    }
    return 0;
}

static int
kfgx_cli_parser_impl(struct kfgx_cmd_struct *cmd, const char **args_set)
{
    return kfgx_cli_tokenizer_impl(cmd, args_set);
}

/**
 * @brief Tokenize CLI arguments.
 *
 * @param[out] cmd       Parsed command structure.
 * @param[in]  args_nr   Number of arguments.
 * @param[in]  args_set  NULL-terminated argument array.
 *
 * @return 0 on success, -1 on error.
 */
int kfgx_cli_tokenizer(struct kfgx_cmd_struct *cmd, const char **args_set)
{
    if (check_cli_args(cmd, args_set))
        return -1;

    return kfgx_cli_tokenizer_impl(cmd, args_set);
}

/**
 * @brief Parse command-line arguments into a token list.
 *
 * @param[out] cmd       Parsed command structure.
 * @param[in]  args_nr   Number of arguments.
 * @param[in]  args_set  Argument array.
 *
 * @return 0 on success, -1 on error.
 */
int kfgx_cli_parser(struct kfgx_cmd_struct *cmd, char **args_set)
{
    if (cmd->args_nr == 0) {
        pr_debug("args_nr=0");
        return 0;
    }

    if (check_cli_args(cmd, (const char **)args_set))
        return -1;

    return kfgx_cli_parser_impl(cmd, (const char **)args_set);
}