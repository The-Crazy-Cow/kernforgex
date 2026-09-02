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
#include "prio_lists.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Cleans up and removes unmatched token nodes from the linked list.
 *
 * Traverses the token list starting at @p head. For any token node where
 * @c match is 0, the node is unlinked and its memory is freed using
 * @ref kfgx_token_free.
 *
 * @param[in] head Pointer to the head of the token linked list.
 * @return token_t* Pointer to the updated head of the linked list.
 */
static token_t *kfgx_tokens_cleanup_unmatched(token_t *head)
{
    token_t *curr = head;
    token_t *prev = NULL;

    while (curr) {
        token_t *next = curr->next;

        if (!curr->match) {
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }

            curr->next = NULL;
            kfgx_token_free(curr);

            curr = next;
        } else {
            prev = curr;
            curr = next;
        }
    }
    return head;
}

/**
 * @brief Parses CLI arguments and validates matching tokens without internal
 * deallocations.
 *
 * @param[in,out] cmd Structure containing arguments set and handler with target
 * tokens.
 * @return 0 on success, -1 on invalid input or structure.
 */
static int kfgx_cli_tokenizer_impl(struct kfgx_cmd_struct *cmd)
{
    const char **set;
    char *c;
    token_t *t, *lt;

    if (!cmd || !cmd->handler || !cmd->handler->ltokens || !cmd->args_set) {
        pr_warn(
            "Invalid command structure: cmd=%p, handler=%p, args_set=%p",
            (void *)cmd,
            (void *)(cmd ? cmd->handler : NULL),
            (void *)(cmd ? cmd->args_set : NULL));
        return -1;
    }

    set = cmd->args_set;
    lt = cmd->handler->ltokens;

    while (*set) {
        const char *arg = *set;
        c = strchr(arg, '=');

        if (c) {
            size_t key_len = (size_t)(c - arg);
            char *key = malloc(key_len + 1);
            if (!key)
                return -1;

            memcpy(key, arg, key_len);
            key[key_len] = '\0';

            foreach_node(t, lt)
            {
                if (t->opt && HAVE_LONG_OPTION(t->opt->l_opt, key)) {
                    t->match = 1;

                    if (*(c + 1) == '\0') {
                        pr_debug("no value assigned after '='");
                        free(key);
                        goto end;
                    }

                    t->opt->value = strdup(c + 1);
                    pr_debug(
                        "Matched option: %s with value: %s",
                        key,
                        t->opt->value);
                    break;
                }
            }
            free(key);
        } else {
            int matched = 0;

            foreach_node(t, lt)
            {
                pr_debug("%s", t->opt->l_opt);
                if (t->opt && HAVE_LONG_OPTION(t->opt->l_opt, arg)) {
                    t->match = 1;
                    matched = 1;
                    break;
                }
            }

            if (!matched && arg[0] == '-' && arg[1] != '\0' && arg[1] != '-') {
                for (const char *p = arg + 1; *p != '\0'; p++) {
                    char short_str[2] = {*p, '\0'};
                    foreach_node(t, lt)
                    {
                        if (t->opt &&
                            HAVE_SHORT_OPTION(t->opt->s_opt, short_str)) {
                            t->match = 1;
                            break;
                        }
                    }
                }
            }
        }
        set++;
    }

end:
    cmd->handler->ltokens = kfgx_tokens_cleanup_unmatched(lt);
    return 0;
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
int kfgx_cli_tokenizer(struct kfgx_cmd_struct *cmd)
{
    if (check_cli_args(cmd))
        return -1;

    return kfgx_cli_tokenizer_impl(cmd);
}

static int kfgx_cli_parser_impl(struct kfgx_cmd_struct *cmd)
{
    return kfgx_cli_tokenizer_impl(cmd);
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
int kfgx_cli_parser(struct kfgx_cmd_struct *cmd)
{
    if (cmd->args_nr == 0) {
        pr_debug("args_nr=0");
        return 0;
    }

    if (!cmd->handler) {
        pr_warn("handler=%p", (void *)cmd->handler);
        return -1;
    }

    return kfgx_cli_parser_impl(cmd);
}