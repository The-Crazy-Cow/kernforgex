// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

/**
 * @file      test_parser.c
 * @author    jd
 * @brief     Test suite for CLI argument parsing and tokenization verification.
 * @version   0.1
 * @date      2026-08-30
 *
 * @details   This test suite automatically validates the tokenizer and parser
 *            functions (`kfgx_cli_tokenizer` and `kfgx_cli_parser`) by checking
 *            flag handling, key-value option parsing, and memory safety.
 *
 * @copyright GNU General Public License v2.0
 */

#include "parser.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char *test_name;
    int argc;
    const char *argv[10];
    int expected_token_count;
    const char *expected_options[5];
    const char *expected_values[5];
} t_test_case;

static void run_single_test(const t_test_case *tc)
{
    struct kfgx_cmd_struct cmd = {0};

    printf("[TEST] %s\n", tc->test_name);

    int res = kfgx_cli_parser(&cmd, tc->argc, (char **)tc->argv);
    assert(res == 0 && "kfgx_cli_parser failed unexpectedly");

    token *t = cmd.ltokens;
    int count = 0;

    while (t) {
        printf("  option: %-15s", t->option ? t->option : "(null)");
        if (t->value) {
            printf(" | value: %s", t->value);
        }
        printf("\n");

        assert(count < tc->expected_token_count && "Too many tokens returned");

        if (tc->expected_options[count]) {
            assert(t->option != NULL);
            assert(strcmp(t->option, tc->expected_options[count]) == 0);
        } else {
            assert(t->option == NULL);
        }

        if (tc->expected_values[count]) {
            assert(t->value != NULL);
            assert(strcmp(t->value, tc->expected_values[count]) == 0);
        } else {
            assert(t->value == NULL);
        }

        count++;
        t = t->next;
    }

    assert(count == tc->expected_token_count && "Token count mismatch");

    kfgx_cli_token_free(cmd.ltokens);

    printf("Result: OK\n\n");
}

int main(void)
{
    t_test_case suite[] = {
        {.test_name = "Single flag option",
         .argc = 1,
         .argv = {"-v", NULL},
         .expected_token_count = 1,
         .expected_options = {"-v"},
         .expected_values = {NULL}},
        {.test_name = "Key-value options",
         .argc = 2,
         .argv = {"-foo=bar", "--level=42", NULL},
         .expected_token_count = 2,
         .expected_options = {"-foo", "--level"},
         .expected_values = {"bar", "42"}},
        {.test_name = "Mixed flags and values",
         .argc = 3,
         .argv = {"--verbose", "-speed=fast", "file.txt", NULL},
         .expected_token_count = 3,
         .expected_options = {"--verbose", "-speed", "file.txt"},
         .expected_values = {NULL, "fast", NULL}}};

    size_t total_tests = sizeof(suite) / sizeof(suite[0]);

    printf("=== Running Kernforgex Parser Test Suite ===\n\n");
    for (size_t i = 0; i < total_tests; i++) {
        run_single_test(&suite[i]);
    }
    printf("All %zu tests passed successfully!\n", total_tests);

    return 0;
}