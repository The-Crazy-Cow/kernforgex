// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

#ifndef KERNFORGE_PARSER_H
#define KERNFORGE_PARSER_H

#include "clicntl.h"

int kfgx_cli_parser(struct kfgx_cmd_struct *, char **);
int kfgx_cli_tokenizer(struct kfgx_cmd_struct *, const char **);
void kfgx_cli_token_free(token *tokens_list); // tokens list
int set_token_dirty(token *t);

#endif /* KERNFORGE_PARSER_H */