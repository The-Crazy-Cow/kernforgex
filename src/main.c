// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

#include <stdio.h>
#include <string.h>
#include "parser.h"

int main(int argc, char *argv[], char *envp[])
{
    (void)envp;

    struct kfgx_cmd_struct cmd = {0};

    if (kfgx_cli_parser(&cmd, argc - 1, (const char **)(argv + 1))) {
        fprintf(stderr, "Error: Parsing failed\n");
        return -1;
    }

    // ------ testing purpose
    token *t = cmd.ltokens;
    while (t) {
        if (t->option)
            printf("option: %s", t->option);

        if (t->value)
            printf(" | value: %s", t->value);

        printf("\n");
        t = t->next;
    }
    // ------------------------

    kfgx_cli_token_free(cmd.ltokens);

    return 0;
}