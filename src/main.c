// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

#include "clicntl.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[])
{
    (void)envp;

    struct kfgx_cmd_struct cmd = {
        .ltokens = NULL,
        .args_nr = argc,
        .flags = 0,
        .ret = 0,
    };

    if (kfgx_cli_parser(&cmd, (argv + 1))) {
        fprintf(stderr, "Error: Parsing failed\n");
        return -1;
    }

#ifdef DEBUG
    token *t = cmd.ltokens;
    while (t) {
        if (t->option)
            printf("option: %s", t->option);

        if (t->value)
            printf(" | value: %s", t->value);

        printf("\n");
        t = t->next;
    }
    //--------------------------------------
#endif
    kfgx_execute_handler(&cmd);
    kfgx_cli_token_free(cmd.ltokens);

    return 0;
}