// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

#include "clicntl.h"
#include "handers.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[])
{
    (void)envp;
    if (kfgx_init_handlers()) {
        return -1;
    }

    struct kfgx_cmd_struct cmd = {
        .ltokens = NULL,
        .args_nr = argc,
        .flags = 0,
        .ret = 0,
    };

    // insert our default handler
    if (kfgx_handler_init()) {
        return -1;
    }

    if (kfgx_cli_parser(&cmd, (argv + 1))) {
        fprintf(stderr, "Error: Parsing failed\n");
        return -1;
    }

    kfgx_execute_handler(&cmd);
    kfgx_cli_token_free(cmd.ltokens);

    return 0;
}