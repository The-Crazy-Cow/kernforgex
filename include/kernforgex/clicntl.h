// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

#ifndef INCLUDE_KERNFORGE_CLICNTL_H
#define INCLUDE_KERNFORGE_CLICNTL_H

#include "debug.h"
#include <string.h>

// handler
#define HANDLER_NAME_MAX 30

#define HANDLER_ALL 0x0
#define HANDLER_INST 0x2
#define HANDLER_UINST 0x4
#define HANDLER_SETALS 0x8
#define HANDLER_RMALS 0x10
#define HANDLER_VERBOSE 0x12
#define HANDLER_UNKWN 0x14

#define HAVE_SHORT_OPTION(option, a) ((a) == (option))
#define HAVE_LONG_OPTION(option, a) (!strcmp((option), (a)))

typedef struct {
    const char *long_opt;
    const char *short_opt;
    int has_value;
} kfgx_opt_rule;

// token
#define TOKEN_VALID 0x00
#define TOKEN_DIRTY 0x01

typedef struct token {
    char *option;
    char *value;
    unsigned int free : 1;
    struct token *next;
} token;

struct kfgx_cmd_struct {
    token *ltokens; // tokens list
    const int args_nr;
    int flags;
    int ret;
};

struct handler {
    char name[HANDLER_NAME_MAX];            // handler
    char name_flag[2];                      // -h
    char flag_option[HANDLER_NAME_MAX + 2]; // --handler
    void (*usage)(void);
    int (*handler)(struct kfgx_cmd_struct *);
};

int get_token_flags(token *);
int kfgx_get_handler(struct kfgx_cmd_struct *cmd);
int kfgx_execute_handler(struct kfgx_cmd_struct *cmd);

#endif /*INCLUDE_KERNFORGE_CLICNTL_H*/