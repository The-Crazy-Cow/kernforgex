// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

#ifndef KERNFORGE_PARSER_H
#define KERNFORGE_PARSER_H

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
};

int kfgx_cli_parser(struct kfgx_cmd_struct *, int, char **);
int kfgx_cli_tokenizer(struct kfgx_cmd_struct *, const int, const char **);
void kfgx_cli_token_free(token *tokens_list); // tokens list
int set_token_dirty(token *t);

#endif /* KERNFORGE_PARSER_H */