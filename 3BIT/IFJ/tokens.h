/**
 * Tokenizer
 * IFJ 2017 - 1. projekt
 *
 * Autoři: xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
 *          xfirca00 (Anton Firc), xliska16 (Jakub Liška)
 **/

#ifndef tokens_h
#define tokens_h

#include <stdio.h>

typedef enum {
    token_invalid,
    token_equals,
    token_plus,
    token_minus,
    token_multiply,
    token_divide,
    token_divide_double,
    token_less,
    token_less_or_equal,
    token_not_equal,
    token_more,
    token_more_or_equal,
    token_bracket_left,
    token_bracket_right,
    token_underscore,
    token_comment,
    token_comment_start,
    token_comment_end,
    token_eol,
    token_eof,
    token_keyword,
    token_function_call,
    token_identifier,
    token_identifier_function,
    token_var_name,
    token_int,
    token_double,
    token_expression,
    token_semicolon,
    token_string_literal,
    token_var_type,
    token_comma,
    token_non_terminal,
    token_not_a_number,
} Token_Type;

typedef struct {
    Token_Type type;
    int length;
    char data[128];
} Token;

typedef enum {
    state_new,
    state_keyword,
    state_string_literal,
    state_identifier,
    state_var,
    state_digit,
    state_expression,
    state_function_arguments,
    state_function_return_type,
    state_double_exp,
    state_double_dec,
} State;
void token_data(Token *token, char *data) ;
Token * token_get(int *line_number);
Token * token_init();
// test expr_eval
void token_data(Token *token, char *data);


#endif /* tokens_h */
