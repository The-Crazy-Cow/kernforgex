// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

#include "handlers.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

#define STRICT_MODE

int main(int argc, char *argv[], char *envp[])
{
    (void)envp;
    if (kfgx_init_handlers()) {
        return -1;
    }

    struct kfgx_cmd_struct cmd = {
        .handler = NULL,
        .args_nr = argc - 1, // Exclude the program name
        .ret = 0,
        .args_set = (const char **)argv + 1, // Skip the program name
    };

    // insert our default handler
    if (kfgx_handler_init()) {
        return -1;
    }

    if (kfgx_handle(&cmd)) {
        fprintf(stderr, "Error: Parsing failed\n");
        return -1;
    }

    return 0;
}